/*
   AudioEffectBTNRH

   Created: Ira Ray Jenkins, Creare, June 2021
            Chip Audette, OpenAudio, August 2021

   Based on MyAudioAlgorithm by Chip Audette, OpenAudio April 2017
   Purpose: Demonstrate a port of CHAPRO algorithm to Tympan

   MIT License.  use at your own risk.
*/

#ifndef _AudioEffectBTNRH_h
#define _AudioEffectBTNRH_h

// Libraries that all Tympan Audio Effects should include
#include <Arduino.h>
#include "AudioStream_F32.h"

// Algorithm-specific include files
#include <chapro.h>
#include "test_gha.h"  ////////////////////////////////////////// Update this for your CHAPRO Algorithm!!!!


class AudioEffectBTNRH : public AudioStream_F32
{
public:
    //constructor
    AudioEffectBTNRH(const AudioSettings_F32 &settings) : AudioStream_F32(1, inputQueueArray_f32){};
    
    //CHAPRO-relevant data members...each instance of this algorithm gets its own copy of these data structures
    void *cp[NPTR] = {0};  //Create a local version of CHA_PTR for use by each instance of this class.  NPTR is set in chapro.h???
    I_O io;                //Create a local version of I_O for use by each instance of this class.
    
    double get_cha_dvar(int ind) { return ((double *)cp[_dvar])[ind]; }; 
    double set_cha_dvar(int ind, double val) { return ((double *)cp[_dvar])[ind] = val; };
    int get_cha_ivar(int ind) { return ((int *)cp[_ivar])[ind]; }; 
    int set_cha_ivar(int ind, int val) { return ((int *)cp[_ivar])[ind] = val; };
    

    //setup methods
    void setup(void)  { 
      Serial.println("AudioEffectBTNRH: setup(): BTNRH configure...");
      configure(&io);               //in test_nfc.h
    
      Serial.println("AudioEffectBTNRH: setup(): BTNRH prepare...");
      prepare(&io, cp);             //in test_nfc.h

      Serial.println("AudioEffectBTNRH: setup(): _dvar = " + String(_dvar) + ", _rho = " + String(_rho) + ", ((double *)cp[_dvar])[_rho] = " + String(((double *)cp[_dvar])[_rho],9) + ", get_cha_dvar(_rho) = " + String(get_cha_dvar(_rho),9));
      Serial.println("AudioEffectBTNRH: setup(): CHA_IVAR[_mxl] = " + String(get_cha_ivar(_mxl)));
      Serial.println("AudioEffectBTNRH: setup(): CHA_IVAR[_in1] = " + String(get_cha_ivar(_in1)) + ", CHA_IVAR[_in2] = " + String(get_cha_ivar(_in2)));
    }    
    

    // ////////////////////////////////////////// Here is the call into the CHAPRO that actually does the signal processing
    // Here is where you can add your algorithm.  This function gets called block-wise
    void applyMyAlgorithm(audio_block_f32_t *audio_block)
    {
        float *x = audio_block->data;  //This is used input audio.  And, the output is written back in here, too
        int cs = audio_block->length;  //How many audio samples to process?

        //hopefully, this one line is all that needs to change to reflect what CHAPRO code you want to use
        process_chunk(cp, x, x, cs); //see test_gha.h  (or whatever test_xxxx.h is #included at the top)

    } //end of applyMyAlgorithms
    // /////////// End of the signal processing code that references CHAPRO

   

    // ////////////////////////////////////////// Here is some housekeeping stuff that (hopefully) never needs to be touched

    //here's the method that is called automatically by the Teensy Audio Library for every audio block that needs to be processed
    void update(void)
    {
        if (!enabled) return;
      
        //Serial.println("AudioEffectMine_F32: doing update()");  //for debugging.
        audio_block_f32_t *audio_block;
        audio_block = AudioStream_F32::receiveWritable_f32();
        if (!audio_block) return;

        //do your work
        applyMyAlgorithm(audio_block); //this is the method defined earlier that you can touch as you see fit

        ///transmit the block and release memory
        AudioStream_F32::transmit(audio_block);
        AudioStream_F32::release(audio_block);
    }

    bool setEnabled(bool val = true) { return enabled = val; }
    
private:
    //state-related variables
    audio_block_f32_t *inputQueueArray_f32[1]; //memory pointer for the input to this module
    bool enabled = false;

}; //end class definition for AudioEffectBTNRH

#endif
