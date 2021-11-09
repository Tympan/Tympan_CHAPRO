/*
   chapro_test_GHA_serial

   Created: Chip Audette, OpenAudio, November 2021

   Purpose: Demonstrate a port of tst_gha.c from ChaPro to Tympan.
            Includes ability to control some of the AFC algorithm via the SerialMonitor or App
            and to plot some of the AFC states via the SerialPlotter.

   MIT License.  use at your own risk.
*/


//Include Arduino/Tympan related libraries
#include <Arduino.h>
#include <Tympan_Library.h>
#include "implementStdio.h"   //alows for printf

//Include algorithm-specific files
#include "test_gha.h"          //see the tab "test_gha.h"..........be sure to update the name if you change the filename!
#include "AudioEffectBTNRH.h"  //see the tab "AudioEffectBTNRH.h"
 
// ///////////////////////////////////////// setup the audio processing classes and connections

//set the sample rate and block size
const float sample_rate_Hz = (int)srate;  //Set in test_gha.h.  Must be one of the values in the table in AudioOutputI2S_F32.h
const int audio_block_samples = chunk;    //Set in test_gha.h.  Must be less than or equal to 128 
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

// Create the Tympan
Tympan myTympan(TympanRev::E, audio_settings);

//create audio objects
AudioInputI2S_F32     audio_in(audio_settings);
AudioEffectBTNRH_F32  BTNRH_alg1(audio_settings);  //see tab "AudioEffectBTNRH.h"
AudioEffectGain_F32   gain1(audio_settings);       //added gain block to easily increase or lower the gain
AudioOutputI2S_F32    audio_out(audio_settings);
AudioSDWriter_F32_UI  audioSDWriter(audio_settings); //this is 2-channels of audio by default, but can be changed to 4 in setup()


//connect the inputs to the BTNRH alg
AudioConnection_F32   patchCord1( audio_in,   0, BTNRH_alg1, 0);  //left input
AudioConnection_F32   patchCord2( BTNRH_alg1, 0, gain1, 0);

//connect the BTNRH alg to the outputs
AudioConnection_F32   patchCord11(gain1, 0, audio_out,  0);  //left output
AudioConnection_F32   patchCord12(gain1, 0, audio_out,  1);  //right output, same as left

//connect to the SD writer
AudioConnection_F32   patchCord21(audio_in, 0, audioSDWriter,  0);  //left will be the raw input
AudioConnection_F32   patchCord22(gain1,    0, audioSDWriter,  1);  //right will be the processed output


// Create classes for controlling the system
#include      "SerialManager.h"
#include      "State.h"                            
BLE_UI        ble(&myTympan);                                      //create bluetooth BLE
SerialManager serialManager(&ble);                                 //create the serial manager for real-time control (via USB or App)
State         myState(&audio_settings, &myTympan, &serialManager); //keeping one's state is useful for the App's GUI

//set up the serial manager
void setupSerialManager(void) {
  //register all the UI elements here  
  serialManager.add_UI_element(&myState);
  serialManager.add_UI_element(&ble);
  //serialManager.add_UI_element(&earpieceMixer); 
  serialManager.add_UI_element(&audioSDWriter);
}

float setDigitalGain_dB(float val_dB) {
    return myState.digital_gain_dB = gain1.setGain_dB(val_dB);
}

// /////////////////////////  Start the Arduino-standard functions: setup() and loop()

void setup() { //this runs once at startup  
  Serial1.begin(9600); //for talking to the Bluetooth unit (9600 is the right choice for for RevE)
  while ((!Serial) && (millis() < 1000));  //stall for a bit to let the USB get up and running.
  //myTympan.beginBothSerial(); delay(500);
      
  if (Serial) Serial.print(CrashReport);  //if it crashes and restarts, this will give some info
  myTympan.println("CHAPRO for Tympan: Test GHA: setup():...");
  myTympan.println("  Sample Rate (Hz): " + String(audio_settings.sample_rate_Hz));
  myTympan.println("  Audio Block Size (samples): " + String(audio_settings.audio_block_samples));

  // Audio connections require memory to work.
  AudioMemory_F32(50, audio_settings);

  // /////////////////////////////////////////////  do any setup of the algorithms

  BTNRH_alg1.setup();           //in AudioEffectBTNRH.h
  BTNRH_alg1.setEnabled(true);  //see AudioEffectBTNRH.h.  This could be done later in setup()
 
  // //////////////////////////////////////////// End setup of the algorithms


  // Start the Tympan
  Serial.println("setup: Tympan enable...");
  myTympan.enable();

  //setup DC-blocking highpass filter running in the ADC hardware itself
  float cutoff_Hz = 200.0;  //set the default cutoff frequency for the highpass filter
  myTympan.setHPFonADC(true,cutoff_Hz,audio_settings.sample_rate_Hz); //set to false to disble

  //Choose the desired input
  myTympan.inputSelect(TYMPAN_INPUT_ON_BOARD_MIC);     // use the on board microphones
  //myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_MIC);    // use the microphone jack - defaults to mic bias 2.5V
  //myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_LINEIN); // use the microphone jack - defaults to mic bias OFF
  
  //Set the desired volume levels
  myTympan.volume_dB(myState.output_gain_dB);    // headphone amplifier.  -63.6 to +24 dB in 0.5dB steps.
  float input_gain_dB = 15.0;                    //only applied to analog sources, not the digital earpieces
  myTympan.setInputGain_dB(input_gain_dB);       // set input volume, 0-47.5dB in 0.5dB setps

 //setup the serial manager
  setupSerialManager();

  //set the default gain
  Serial.println("setup(): setting overall gain to " + String(myState.digital_gain_dB));
  setDigitalGain_dB(myState.digital_gain_dB);

  //prepare the SD writer for the format that we want and any error statements
  audioSDWriter.setSerial(&myTympan);
  //audioSDWriter.setNumWriteChannels(4);     // Default is 2 channels.  Can record 4 channel, if asked
  Serial.println("Setup: SD configured for writing " + String(audioSDWriter.getNumWriteChannels()) + " audio channels.");
  
  //setup BLE
  //ble.setUseFasterBaudRateUponBegin(true); //speeds up baudrate to 115200.  ONLY WORKS FOR ANDROID.  If iOS, you must set to false.
  delay(500); ble.setupBLE(myTympan.getBTFirmwareRev()); delay(500); //Assumes the default Bluetooth firmware. You can override!

  //finish setup
  Serial.println("Setup complete.");
  serialManager.printHelp();
}

void loop() {  //this runs forever in a loop

  //respond to Serial commands
  while (Serial.available()) serialManager.respondToByte((char)Serial.read());   //USB...respondToByte is in SerialManagerBase...it then calls SerialManager.processCharacter(c)

  //respond to BLE
  if (ble.available() > 0) {
    String msgFromBle; int msgLen = ble.recvBLE(&msgFromBle);
    for (int i=0; i < msgLen; i++) serialManager.respondToByte(msgFromBle[i]);  //respondToByte is in SerialManagerBase...it then calls SerialManager.processCharacter(c)
  }

  //service the BLE advertising state
  ble.updateAdvertising(millis(),5000); //check every 5000 msec to ensure it is advertising (if not connected)

  //service the SD recording
  audioSDWriter.serviceSD_withWarnings(audio_in); //needs some info from i2s_in to provide some of the warnings

  //service the LEDs...blink slow normally, blink fast if recording
  myTympan.serviceLEDs(millis(),audioSDWriter.getState() == AudioSDWriter::STATE::RECORDING); 

  //periodically print the CPU and Memory Usage
  if (myState.flag_printCPUandMemory) myState.printCPUandMemory(millis(), 3000); //print every 3000msec  (method is built into TympanStateBase.h, which myState inherits from)
  if (myState.flag_printCPUandMemory) myState.printCPUtoGUI(millis(), 3000);     //send to App every 3000msec (method is built into TympanStateBase.h, which myState inherits from)

  if (myState.flag_printLeftFeedbackModel) servicePrintingFeedbackModel(millis(), 1000, BTNRH_alg1);
  if (myState.flag_printLeftFeedbackModel_toApp) servicePrintingFeedbackModel_toApp(millis(), 1000, BTNRH_alg1); //BLE transfer is slow, this this ends up spacing transmissions as starting a new one after 1000msec has passed since end of previous one
  //if (myState.flag_printRightFeedbackModel) servicePrintingFeedbackModel(millis(),1000, BTNRH_alg2);
}

// ////////////////////////////////////////////////////////////// Servicing routines

void servicePrintingFeedbackModel(unsigned long curTime_millis, unsigned long updatePeriod_millis, AudioEffectBTNRH_F32 &alg) {
  static unsigned long lastUpdate_millis = 0;
  //has enough time passed to update everything?
  if (curTime_millis < lastUpdate_millis) lastUpdate_millis = 0; //handle wrap-around of the clock
  if ((curTime_millis - lastUpdate_millis) >= updatePeriod_millis) { //is it time to update the user interface?

    int n_coeff = alg.get_cha_ivar(_afl);
    if (n_coeff > 0) {
      //Serial.println("servicePrintingFeedbackModel: printing feedback model for AFC...");
      float scale_fac = 100.0;  //choose whatever to make the plot prettier
      int n_decimals = 3;
      Serial.println("Model_" + String((int)(scale_fac + 0.5)) + ":");

      Serial.println(scale_fac,n_decimals);
      Serial.println(-scale_fac,n_decimals);
      //Serial.println(0.0);   
      float *efbp = (float *)(alg.cp[_efbp]);  //get a simpler name for the array that we're going to print
      for (int i=0; i<n_coeff; i++) { 
        Serial.println(scale_fac*efbp[i],n_decimals); //print x decimal places
      }
    }

    lastUpdate_millis = curTime_millis; //we will use this value the next time around.
  }
}

void servicePrintingFeedbackModel_toApp(unsigned long curTime_millis, unsigned long updatePeriod_millis, AudioEffectBTNRH_F32 &alg) {
  static unsigned long lastUpdate_millis = 0;
  //has enough time passed to update everything?
  if (curTime_millis < lastUpdate_millis) lastUpdate_millis = 0; //handle wrap-around of the clock
  if ((curTime_millis - lastUpdate_millis) >= updatePeriod_millis) { //is it time to update the user interface?
    int n_coeff = alg.get_cha_ivar(_afl);
    if (n_coeff > 0) {
      Serial.println("servicePrintingFeedbackModel: printing feedback model for AFC...");
      float scale_fac = 1.0;  //choose whatever to make the plot prettier
      int n_decimals = 3;
      String line_prefix = String("P");

      //build up the string with all the data
      ble.sendMessage(line_prefix + String(scale_fac,n_decimals) + String('\n'));
      ble.sendMessage(line_prefix + String(-scale_fac,n_decimals) + String('\n'));
      //ble.sendMessage(line_prefix + String(0.0,n_decimals) + String('\n'));      
      float *efbp = (float *)(alg.cp[_efbp]); //get a simpler name for the array that we're going to print
      for (int i=0; i<n_coeff; i++) { 
        ble.sendMessage(line_prefix + String(scale_fac*efbp[i],n_decimals) + String('\n')); //print x decimal places
      }
      //Serial1.flush();
      //Serial.println("servicePrintingFeedbackModel_toApp: finished.");
    }
    //lastUpdate_millis = curTime_millis; //we will use this value the next time around.
    lastUpdate_millis = millis(); //change the timing so that there are XXXX msec *between* the end of one transmission and the start of the next
  }
}
