# Tympan_CHAPRO
 CHAPRO Examples that run on the Tympan RevE

## SETUP Step 1: Install the Arduino IDE and Teensyduino and the Tympan_Library

To program for the Tympan, you need three sets of tools, as described in our [Getting Started](https://github.com/Tympan/Docs/wiki/Getting-Started-with-Tympan#install-software) documents.  You should use that link as there are links and screenshots.

The short version is:
1) Install the [Arduino IDE](https://www.arduino.cc/en/software).  
    * We're currently at Version 1.8.15, though older versions might work.
3) Install the [Teensyduino](https://www.pjrc.com/teensy/td_download.html) add-on for Teensy based products.
    * We're at V1.54.  Older versions will NOT work.
5) Download the latest [Tympan_Library](https://github.com/Tympan/Tympan_Library) repository
    * Clone (or manually download and unzip).
    * Save to your Arduino libraries directory.  On windows it should end up being Documents\Arduino\Libraries\Tympan_Library

## SETUP Step 2: Get the special branch of the CHAPRO Repository

This repository depends upon a Arduino-enabled version of the BTNRH CHAPRO library.  Use the `tympan` branch of our fork of the CHAPRO library here: https://github.com/Tympan/chapro/tree/tympan

You must clone (or manually download and unzip) this specific branch of the chapro repo into your Arduino libraries directory.  On a Windows computer, the Arduino libraries directory is in Documents\Arduino\Libraries.  The final path, therefore, should be something like Documents\Arduino\Libraries\chapro

If we find that this special version of the CHAPRO repository works, we will do a pull request to pull the handful of changes back into the main CHAPRO repository that is owned by BTNRH.

## SETUP Step 3: Get this Tympan_CHAPRO Repository

The base CHAPRO repository has no example programs that work directly with the Tympan.  Instead, we need Tympan-specific examples.  These examples invoke only signal processing functions from CHAPRO.  These example programs simply provide the minimal outer structure that an Arduino and Tympan program require.

To get the repo, clone (or manually download and unzip) this Tympan_CHAPRO directory to your computer, preferably in the typical location that the Arduino IDE expects to find Arduino sketches.  On a Windows computer, it would be in Documents\Arduino.  The final path, therefore, should be something like Documents\Arduino\Tympan_CHAPRO.

You can then open these examples in the Arduino IDE via the "File Menu" and "Open".

## COMPATIBLE HARDWARE

**Tympan Version:** These sketches are made for Tympan RevE.  They will compile and start to run on a Tympan RevD, but the Tympan RevD is too slow to execute most of the algorithms as they are written in CHAPRO.  

**Tympan Earpieces:** Initially (as of 8/26/2021) these sketches do not employ the Tympan earpieces.  They default to using the microphones built into the hardware, so they will be noisy.  The processed audio is presented via the black headphone jack.

We plan on adding support for the earpieces shortly.

## COMPILING

If you have never compiled and run a Tympan program before, you should see the Tympan Getting Started docs.  The short version, however, is:

1) Start the Arduino IDE
    * Open one of these Tympan_CHAPRO programs ("sketches")
    * On Windows, you hopefully placed them in Documents\Arduino\Tympan_CHAPRO

2) Tell the Arduino IDE that you are compiling for Tympan RevE (which is a Teensy 4.1)
    * Under the "Tools" menu, choose "Board", "Teensyduino", "Teensy 4.1"

3) Compile your program and upload to the Tympan
    * At any point, plug in the Tympan via USB and turn it on
    * Under the "Sketch" menu, press "Upload" (which will compile and upload to the Tympan)
    * Alternatively, press the right-arrow in the toolbar.
    * If it compiles successfully, it will upload the program to the Tympan

4) Watch it run!
    * After a couple of seconds, you should hear audio playing through the Tympan (if you plug in headphones!)

5) Debugging via Serial Monitor
    * The Arduino Serial Monitor shows messages printed by the Tympan to help with debugging
    * Go under the "Tools" menu and choose "Serial Monitor"
    * Alternatively, press the magnifying glass button on the far right of the Arduino IDE window

## MAKING YOUR OWN EXAMPLES

CHAPRO includes many test programs, including files like tst_nfc.c and tst_gha.c, which were the basis for the examples in this repo.  The process for making your own examples starts with these tst_xxxx.c examples in CHAPRO.  For these instructions, let's assume that you want to make a Tympan example based on the CHAPRO tst_gfsc.c program.

**1) Start from an Existing Example**: Start from an existing Tympan_CHAPRO example, such as chapro_test_GHA.  Open it in the Arduino IDE and then do "Save As.." to save it under a new name.  If we are conferting tst_gfsc.c, then choose a name like chapro_test_gfsc.  (Be aware that the Arduino IDE will automatically create the directory for you and it will automatically copy over the other files, too.)

**2) Add a New test_xxxx.h File**:  If you are starting with chapro_test_GHA, note that there is a file test_gha.h.  That file built from the CHAPRO file tst_gha.c.  We are going to follow the same pattern for your new program.  If you are converting tst_gfsc.c, create a new file that you call tst_gfsc.h.  You create new files in the Arduino IDE using the little down-carrot button on the far right of the Arduino window.

**3) Copy-Paste Your Code and Comment Out Unneeded Items**:  Open your own CHAPRA file (tst_gfsc.c) and copy-paste the contents into your new file (test_gfsc.h).  Then, following the pattern the in the existing example (test_gha.h), comment out all the lines that are not needed for making it run on the Tympan.  Finally add the `#ifndef` and `#define` at the top of the file and add #endif at the end.  Again, use test_gha.h as a model.

**4) Point to Your test_xxxx.h File**:  In the other tabs of your sketch, note the #include lines at the top.  Find the ones that point to test_gha.h and point them to your new file (test_gfsc.h).

**5) Adapt AudioEffectBTNRH.h**: Click on the tab or AudioEffectBTNRH.  This is the file that calls upon functions in your new test_gfsc.h.  In particular, look at the method called `setup()` where it calls the functions `configure()` and `prepare()`.  Is this still correct for the function names in your test_gsfc.h?  Similarly, scroll a bit further down to find the method called `applyMyAlgorithm()`.  Note that it calls `process_chunk()`.  Is this still correct for the function names in your test_gsfc.h?  Change the function names here in AudioEffectBTNRH.h (or in test_gsfc.h) as needed to make everything agree.

**6) Delete Your Old test_xxxx.h File**:  When you're done with your old test_gha.h file, you can remove it from the sketch.  You can do this by deleting it.  Go under the down-carrot button on the far right of the Arduino window and choose "Delete".

**7) Copile and Debug**:  You're done!  Now you can iteratively compile and debug until it runs.  Since there is no deubber in Arduino, you must debug by printing lines to the Serial Monitor.  You can use the Arduino `println()` command, like: `Serial.println("My string...and here is a number: " + String(i));`.  Or you can use trusty old `printf()`.

Good luck!

## MIXING CHAPRO AND TYMPAN ALGORITHMS

You can absolutely mix CHAPRO and Tympan audio processing elements.  In these examples, you will note that the CHAPRO algorithm has been wrapped into AudioEffectBTNRH.  This makes the CHAPRO algorithm look like any other Tympan audio class.  Therefore, you can plug in any othre Tympan audio classes before, after, or in parallel with the CHAPRO elements.

As a basic example, here is how you might insert an adjustable gain block before the CHAPRO block:

```
//create audio objects
AudioInputI2S_F32     audio_in(audio_settings);    
AudioEffectGain_F32   gain1(audio_settings);       //here is a block from Tympan_Library
AudioEffectBTNRH      BTNRH_alg1(audio_settings);  //see tab "AudioEffectBTNRH.h"
AudioOutputI2S_F32    audio_out(audio_settings);   //always list last

//connect the inputs to the gain alg
AudioConnection_F32   patchCord1(audio_in,   0, gain1, 0);  //left input

//connect the gain alg to the BTNRH alg
AudioConnection_F32   patchCord2(gain1, 0, BTNRH_alg1, 0);  //processing

//connect the BTNRH alg to the outputs
AudioConnection_F32   patchCord3(BTNRH_alg1, 0, audio_out,  0);  //left output
AudioConnection_F32   patchCord4(BTNRH_alg1, 0, audio_out,  1);  //right output, same as left
```

You can do this with any Tympan audio class.

