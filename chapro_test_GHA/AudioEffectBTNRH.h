

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
    
    //CHAPRO-relevant data members
    void *cp[NPTR] = {0};  //Create a local version of CHA_PTR for use by each instance of this class.  NPTR is set in chapro.h???
    I_O io;                //Create a local version of I_O for use by each instance of this class.

    //setup methods
    void setup(void)  { 
      Serial.println("AudioEffectBTNRH: setup(): BTNRH configure...");
      configure(&io);               //in test_gha.h
    
      Serial.println("AudioEffectBTNRH: setup(): BTNRH prepare...");
      prepare(&io, cp);             //in test_gha.h
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

}; //end class definition 
