
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
AudioInputI2S_F32   audio_in(audio_settings);
AudioEffectBTNRH    BTNRH_alg1(audio_settings);  //for processing the left.  See tab "AudioEffectBTNRH.h"
AudioEffectBTNRH    BTNRH_alg2(audio_settings);  //for processing the right.  See tab "AudioEffectBTNRH.h"
AudioOutputI2S_F32  audio_out(audio_settings);

//connect the inputs to the BTNRH alg
AudioConnection_F32 patchCord1( audio_in,   0, BTNRH_alg1, 0);  //left input
AudioConnection_F32 patchCord2( audio_in,   1, BTNRH_alg2, 0);  //right input

//connect the BTNRH alg to the outputs
AudioConnection_F32 patchCord11(BTNRH_alg1, 0, audio_out,  0);  //left output
AudioConnection_F32 patchCord12(BTNRH_alg2, 0, audio_out,  1);  //right output



// /////////////////////////  Start the Arduino-standard functions: setup() and loop()

void setup() { //this runs once at startup
  
    myTympan.beginBothSerial(); delay(500);
    if (Serial) Serial.print(CrashReport);  //if it crashes and restarts, this will give some info
    myTympan.println("CHAPRO for Tympan: Test GHA Stereo: setup():...");
    myTympan.println("  Sample Rate (Hz): " + String(audio_settings.sample_rate_Hz));
    myTympan.println("  Audio Block Size (samples): " + String(audio_settings.audio_block_samples));

    // Audio connections require memory to work.
    AudioMemory_F32(50, audio_settings);

    // /////////////////////////////////////////////  do any setup of the algorithms

    BTNRH_alg1.setup();        //in AudioEffectNFC.h
    BTNRH_alg2.setup();        //in AudioEffectNFC.h

    BTNRH_alg1.setEnabled(true);  //see AudioEffectNFC.h.  This could be done later in setup()
    BTNRH_alg2.setEnabled(true);  //see AudioEffectNFC.h.  This could be done later in setup()
   
    // //////////////////////////////////////////// End setup of the algorithms


    // Start the Tympan
    Serial.println("setup: Tympan enable...");
    myTympan.enable();

    //setup DC-blocking highpass filter running in the ADC hardware itself
    float cutoff_Hz = 40.0;  //set the default cutoff frequency for the highpass filter
    myTympan.setHPFonADC(true,cutoff_Hz,audio_settings.sample_rate_Hz); //set to false to disble

    //Choose the desired input
    myTympan.inputSelect(TYMPAN_INPUT_ON_BOARD_MIC);     // use the on board microphones
    //myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_MIC);    // use the microphone jack - defaults to mic bias 2.5V
    //myTympan.inputSelect(TYMPAN_INPUT_JACK_AS_LINEIN); // use the microphone jack - defaults to mic bias OFF
    
    //Set the desired volume levels
    myTympan.volume_dB(0);                   // headphone amplifier.  -63.6 to +24 dB in 0.5dB steps.
    float input_gain_dB = 15.0;
    myTympan.setInputGain_dB(input_gain_dB); // set input volume, 0-47.5dB in 0.5dB setps

    //finish setup
    Serial.println("Setup complete.");
}

void loop() {  //this runs forever in a loop
  
    //periodically print the CPU and Memory Usage
    myTympan.printCPUandMemory(millis(),1000); //print every 3000 msec

    //stall, just to be nice?
    delay(5);
}
