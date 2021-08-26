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

This repository depends upona Arduino-enabled version of the BTNRH CHAPRO library.  Use the `tympan` branch of our fork of the CHAPRO library here: https://github.com/Tympan/chapro/tree/tympan

You must clone (or manually download and unzip) this specific branch of the chapro repo into your Arduino libraries directory.  On a Windows computer, the Arduino libraries directory is in Documents\Arduino\Libraries.  The final path, therefore, should be something like Documents\Arduino\Libraries\chapro

If we find that this special version of the CHAPRO repository works, we will do a pull request to pull the handful of changes back into the main CHAPRO repository that is owned by BTNRH.

## SETUP Step 3: Get this Tympan_CHAPRO Repository

The base CHAPRO repository has no example programs that work directly with the Tympan.  Instead, we need Tympan-specific examples.  These examples invoke only signal processing functions from CHAPRO.  These example programs simply provide the minimal outer structure that an Arduino and Tympan program require.

To get the repo, clone (or manually download and unzip) this Tympan_CHAPRO directory to your computer, preferably in the typical location that the Arduino IDE expects to find Arduino sketches.  On a Windows computer, it would be in Documents\Arduino.  The final path, therefore, should be something like Documents\Arduino\Tympan_CHAPRO.

You can then open these examples in the Arduino IDE via the "File Menu" and "Open".

## HARDWARE

These sketches are made for Tympan RevE.  Initially (as of 8/26/2021) these sketches do not employ the Tympan earpieces.  They default to using the microphones built into the hardware, so they will be noisy.  The processed audio is presented via the black headphone jack.

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






