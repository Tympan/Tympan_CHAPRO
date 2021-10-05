
/*
*   chapro_test_GHA_stereo_wEarpieces_wApp
*
*   Created: Chip Audette/Haley Geithner, OpenAudio, September 2021
*   Purpose: Uses BTNRH algorithms to process audio from a combination of microphones. 
*            Uses Tympan Earpieces.  Includes App interaction. 
*
*   TympanRemote App: https://play.google.com/store/apps/details?id=com.creare.tympanRemote
*
*   MIT License.  use at your own risk.
*/

//Include Arduino/Tympan related libraries
#include <Arduino.h>
#include <Tympan_Library.h>
#include "implementStdio.h"   //alows for printf

//Include algorithm-specific files
#include "test_gha.h"          //see the tab "test_gha.h"... be sure to update the name if you change the filename!
#include "AudioEffectBTNRH.h"  //see the tab "AudioEffectBTNRH.h"

//set the sample rate and block size
const float sample_rate_Hz = (int)srate;  //Set in test_gha.h.  Must be one of the values in the table in AudioOutputI2S_F32.h
const int audio_block_samples = chunk;    //Set in test_gha.h.  Must be less than or equal to 128 
AudioSettings_F32 audio_settings(sample_rate_Hz, audio_block_samples);

//create audio library objects for handling the audio
Tympan                      myTympan(TympanRev::E, audio_settings);        
EarpieceShield              earpieceShield(TympanRev::E, AICShieldRev::A); //in the Tympan_Library, EarpieceShield is defined in AICShield.h

//Create all the audio classes and make all of the audio connections
#include      "AudioConnections.h"  //see the tab "AudioConnections.h"

// Create classes for controlling the system
#include      "SerialManager.h"
#include      "State.h"                            
BLE_UI        ble(&myTympan);                                      //create bluetooth BLE
SerialManager serialManager(&ble);                                 //create the serial manager for real-time control (via USB or App)
State         myState(&audio_settings, &myTympan, &serialManager); //keeping one's state is useful for the App's GUI

//Let's connect the built-in earpieceMixer to our global State class via the function below.
void connectClassesToOverallState(void) {
  myState.earpieceMixer = &earpieceMixer.state;
}

//Let's also connect that built-in stuff to our serial manager via the function below.  Once this is done,
//most of the GUI stuff will happen in the background without additional effort!
void setupSerialManager(void) {
  //register all the UI elements here           //myState itself has some UI stuff we can use (like the CPU reporting!)
  serialManager.add_UI_element(&earpieceMixer);
  serialManager.add_UI_element(&audioSDWriter);
  serialManager.add_UI_element(&ble);               //ble allows use to ask the status of bt/ble
  serialManager.add_UI_element(&myState);
}

// define the setup() function, the function that is called once when the device is booting
void setup() {
  //begin the serial comms (for debugging)
  myTympan.beginBothSerial(); delay(1000);
  if (Serial) Serial.print(CrashReport);  //if it crashes and restarts, this will give some info
  myTympan.println("CHAPRO for Tympan: Test GHA Earpiece Mixer with App: setup():...");
  myTympan.println("  Sample Rate (Hz): " + String(audio_settings.sample_rate_Hz));
  myTympan.println("  Audio Block Size (samples): " + String(audio_settings.audio_block_samples));


  //allocate the dynamic memory for audio processing blocks
  AudioMemory_F32(50,audio_settings); 

  // /////////////////////////////////////////////  do any setup of the algorithms

  BTNRH_alg1.setup();        //in AudioEffectNFC.h
  Serial.println("setup: finished BTNRH_alg1.setup() ??");
  BTNRH_alg2.setup();        //in AudioEffectNFC.h
  Serial.println("setup: finished BTNRH_alg2.setup() ??");

  BTNRH_alg1.setEnabled(true);  //see AudioEffectNFC.h.  This could be done later in setup()
  BTNRH_alg2.setEnabled(true);  //see AudioEffectNFC.h.  This could be done later in setup()
 
  // //////////////////////////////////////////// End setup of the algorithms

  //Enable the Tympan to start the audio flowing!
  myTympan.enable();                                            // activate the AIC on the main Tympan board
  earpieceShield.enable();                                      // activate the AIC on the earpiece shield
  earpieceMixer.setTympanAndShield(&myTympan, &earpieceShield); //the earpiece mixer must interact with the hardware, so point it to the hardware
  
  //setup the overall state and the serial manager
  connectClassesToOverallState();
  setupSerialManager();

  //Choose the default input
  if (1) {
    //default to the digital PDM mics within the Tympan earpieces
    earpieceMixer.setAnalogInputSource(EarpieceMixerState::INPUT_PCBMICS);  //Choose the desired audio analog input on the Typman...this will be overridden by the serviceMicDetect() in loop(), if micDetect is enabled
    earpieceMixer.setInputAnalogVsPDM(EarpieceMixerState::INPUT_PDM);       // ****but*** then activate the PDM mics
    Serial.println("setup: PDM Earpiece is the active input.");
  } else {
    //default to an analog input
    earpieceMixer.setAnalogInputSource(EarpieceMixerState::INPUT_MICJACK_MIC);  //Choose the desired audio analog input on the Typman...this will be overridden by the serviceMicDetect() in loop(), if micDetect is enabled
    Serial.println("setup: analog input is the active input.");
  }
  
  //Set the Bluetooth audio to go straight to the headphone amp, not through the Tympan software
  myTympan.mixBTAudioWithOutput(true);

  //set volumes
  myTympan.volume_dB(myState.output_gain_dB);  // -63.6 to +24 dB in 0.5dB steps.  uses signed 8-bit
  myTympan.setInputGain_dB(myState.earpieceMixer->inputGain_dB); // set MICPGA volume, 0-47.5dB in 0.5dB setps

  //set the highpass filter on the Tympan hardware to reduce DC drift
  float hardware_cutoff_Hz = 40.0;  //set the default cutoff frequency for the highpass filter
  myTympan.setHPFonADC(true, hardware_cutoff_Hz, audio_settings.sample_rate_Hz); //set to false to disable
  earpieceShield.setHPFonADC(true, hardware_cutoff_Hz, audio_settings.sample_rate_Hz); //set to false to disable
  
  //setup BLE
  while (Serial1.available()) Serial1.read(); //clear the incoming Serial1 (BT) buffer
  ble.setupBLE(myTympan.getBTFirmwareRev());  //this uses the default firmware assumption. You can override!

  //prepare the SD writer for the format that we want and any error statements
  audioSDWriter.setSerial(&myTympan);
  audioSDWriter.setNumWriteChannels(4);      //can record 2 or 4 channels
  Serial.println("Setup(): SD configured for " + String(audioSDWriter.getNumWriteChannels()) + " channels.");

  Serial.println("Setup complete.");
  serialManager.printHelp();
} //end setup()



// define the loop() function, the function that is repeated over and over for the life of the device
void loop() {

  //look for in-coming serial messages (via USB or via Bluetooth)
  if (Serial.available()) serialManager.respondToByte((char)Serial.read());   //USB Serial

  //respond to BLE
  if (ble.available() > 0) {
    String msgFromBle; int msgLen = ble.recvBLE(&msgFromBle);
    for (int i=0; i < msgLen; i++) serialManager.respondToByte(msgFromBle[i]);
  }

  //service the BLE advertising state
  ble.updateAdvertising(millis(),5000); //check every 5000 msec to ensure it is advertising (if not connected)

  //service the SD recording
  audioSDWriter.serviceSD_withWarnings(i2s_in); //For the warnings, it asks the i2s_in class for some info

  //service the LEDs...blink slow normally, blink fast if recording
  myTympan.serviceLEDs(millis(),audioSDWriter.getState() == AudioSDWriter::STATE::RECORDING); 

  //periodically print the CPU and Memory Usage
  if (myState.flag_printCPUandMemory) myState.printCPUandMemory(millis(), 3000); //print every 3000msec  (method is built into TympanStateBase.h, which myState inherits from)
  if (myState.flag_printCPUandMemory) myState.printCPUtoGUI(millis(), 3000);     //send to App every 3000msec (method is built into TympanStateBase.h, which myState inherits from)

} //end loop();
