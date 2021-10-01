
#ifndef _SerialManager_h
#define _SerialManager_h

#include <Tympan_Library.h>
#include "AudioEffectBTNRH.h"
#include "State.h"


//classes from the main sketch that might be used here
extern Tympan myTympan;                    //created in the main *.ino file
extern State myState;                      //created in the main *.ino file
extern EarpieceMixer_F32_UI earpieceMixer; //created in the main *.ino file
extern AudioSDWriter_F32_UI audioSDWriter;
extern AudioEffectBTNRH BTNRH_alg1, BTNRH_alg1;

//
// The purpose of this class is to be a central place to handle all of the interactions
// to and from the SerialMonitor or TympanRemote App.  Therefore, this class does things like:
//
//    * Define what buttons and whatnot are in the TympanRemote App GUI
//    * Define what commands that your system will respond to, whether from the SerialMonitor or from the GUI
//    * Send updates to the GUI based on the changing state of the system
//
// You could achieve all of these goals without creating a dedicated class.  But, it 
// is good practice to try to encapsulate some of these functions so that, when the
// rest of your code calls functions related to the serial communciation (USB or App),
// you have a better idea of where to look for the code and what other code it relates to.
//

//now, define the Serial Manager class
class SerialManager : public SerialManagerBase  {  // see Tympan_Library for SerialManagerBase for more functions!
  public:
    SerialManager(BLE *_ble) : SerialManagerBase(_ble) {};
      
    void printHelp(void);
    void createTympanRemoteLayout(void); 
    void printTympanRemoteLayout(void); 
    bool processCharacter(char c);  //this is called automatically by SerialManagerBase.respondToByte(char c)

    //method for updating the GUI on the App
    void setFullGUIState(bool activeButtonsOnly = false);
    void updateGainDisplay(void);
    void updateCpuDisplayOnOff(void);
    void updateCpuDisplayUsage(void);

  private:

    TympanRemoteFormatter myGUI;  //Creates the GUI-writing class for interacting with TympanRemote App
   
};




void SerialManager::printHelp(void) {  
  Serial.println("SerialManager Help: Available Commands:");
  Serial.println(" h: Print this help");
  Serial.println(" m/M: AFC: incr/decrease mu (current: " + String(BTNRH_alg1.cha_dvar[_mu],6) + ")");
  Serial.println(" r/R: AFC: incr/decrease rho (current: " + String(BTNRH_alg1.cha_dvar[_rho],6) + ")");
  Serial.println(" e/E: AFC: incr/decrease eps (current: " + String(BTNRH_alg1.cha_dvar[_eps],6) + ")");

  //Add in the printHelp() that is built-into the other UI-enabled system components.
  //The function call below loops through all of the UI-enabled classes that were
  //attached to the serialManager in the setupSerialManager() function used back
  //in the main *.ino file.
  SerialManagerBase::printHelp();  ////in here, it automatically loops over the different UI elements issuing printHelp()
  
  Serial.println();
}

//switch yard to determine the desired action...this method is much shorter now that we're using a lot
//of logic that has already been built-into the UI-enabled classes.
bool SerialManager::processCharacter(char c) {  //this is called by SerialManagerBase.respondToByte(char c)
  bool ret_val = true;
  double old_val, new_val;
  
  switch (c) {
    case 'h':
      printHelp(); 
      break;
    case 'm':
      BTNRH_alg2.cha_dvar[_mu] = (BTNRH_alg1.cha_dvar[_mu] *= 2.0);
      BTNRH_alg2.cha_ivar[_in1] = BTNRH_alg1.cha_ivar[_in1] = 0;  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: increasing AFC mu to "); myTympan.println(BTNRH_alg1.cha_dvar[_mu],6);
      break;
    case 'M':
      BTNRH_alg2.cha_dvar[_mu] = (BTNRH_alg1.cha_dvar[_mu] /= 2.0);
      BTNRH_alg2.cha_ivar[_in1] = BTNRH_alg1.cha_ivar[_in1] = 0;  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: decreasing AFC mu to "); myTympan.println(BTNRH_alg1.cha_dvar[_mu],6);
      break;
    case 'r':
      old_val = BTNRH_alg1.cha_dvar[_rho]; new_val = 1.0-((1.0-old_val)/sqrtf(2.0));
      BTNRH_alg2.cha_dvar[_rho] = (BTNRH_alg1.cha_dvar[_rho] = new_val);
      BTNRH_alg2.cha_ivar[_in1] = BTNRH_alg1.cha_ivar[_in1] = 0;  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: increasing AFC rho to "); myTympan.println(new_val,6);
      break;
    case 'R':
      old_val = BTNRH_alg1.cha_dvar[_rho]; new_val = 1.0-((1.0-old_val)*sqrtf(2.0));
      BTNRH_alg2.cha_dvar[_rho] = (BTNRH_alg1.cha_dvar[_rho] = new_val);
      BTNRH_alg2.cha_ivar[_in1] = BTNRH_alg1.cha_ivar[_in1] = 0;  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: decreasing AFC rho to "); myTympan.println(new_val,6);
      break;
    case 'e':
      BTNRH_alg2.cha_dvar[_eps] = (BTNRH_alg1.cha_dvar[_eps] *= sqrtf(10.0));
      BTNRH_alg2.cha_ivar[_in1] = BTNRH_alg1.cha_ivar[_in1] = 0;  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: increasing AFC eps to "); myTympan.println(BTNRH_alg1.cha_dvar[_eps],6);
      break;
    case 'E':
      BTNRH_alg2.cha_dvar[_eps] = (BTNRH_alg1.cha_dvar[_eps] /= sqrtf(10.0));
      BTNRH_alg2.cha_ivar[_in1] = BTNRH_alg1.cha_ivar[_in1] = 0;  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: decreasing AFC eps to "); myTympan.println(BTNRH_alg1.cha_dvar[_eps],6);
      break;    
    case 'J': case 'j':           //The TympanRemote app sends a 'J' to the Tympan when it connects
      printTympanRemoteLayout();  //in resonse, the Tympan sends the definition of the GUI that we'd like
      break;
    default:
      //if the command didn't match any of the commands, it loops through the processCharacter() methods
      //of any UI-enabled classes that were attached to the serialManager via the setupSerialManager() 
      //function used back in the main *.ino file.
      ret_val = SerialManagerBase::processCharacter(c);  //in here, it automatically loops over the different UI elements
      break; 
  }
  return ret_val;
}



// //////////////////////////////////  Methods for defining the GUI and transmitting it to the App

//define the GUI for the App
void SerialManager::createTympanRemoteLayout(void) {
  
  // Create some temporary variables
  TR_Page *page_h;  //dummy handle for a page
  TR_Card *card_h;  //dummy handle for a card

  //Add first page to GUI  (the indentation doesn't matter; it is only to help us see it better)
  page_h = myGUI.addPage("Earpiece Mixer");
      //select inputs
      card_h = earpieceMixer.addCard_audioSource(page_h); //use its predefined group of buttons for input audio source
      
  //Add second page for more control of earpieces
  page_h = earpieceMixer.addPage_digitalEarpieces(&myGUI); //use its predefined page for controlling the digital earpieces

  //Add a page for some other miscellaneous stuff
  page_h = myGUI.addPage("Globals");

    //Add a button group ("card") for the CPU reporting...use a button group that is built into myState for you!
    card_h = myState.addCard_cpuReporting(page_h);

    //Add a button group for SD recording...use a button set that is built into AudioSDWriter_F32_UI for you!
    card_h = audioSDWriter.addCard_sdRecord(page_h);

  //add some pre-defined pages to the GUI (pages that are built-into the App)
  myGUI.addPredefinedPage("serialMonitor");
}


// Print the layout for the Tympan Remote app, in a JSON-ish string
void SerialManager::printTympanRemoteLayout(void) {
    if (myGUI.get_nPages() < 1) createTympanRemoteLayout();  //create the GUI, if it hasn't already been created
    String s = myGUI.asString();
    Serial.println(s);
    ble->sendMessage(s); //ble is held by SerialManagerBase
    setFullGUIState();
}

// //////////////////////////////////  Methods for updating the display on the GUI

void SerialManager::setFullGUIState(bool activeButtonsOnly) {  //the "activeButtonsOnly" isn't used here, so don't worry about it

  //Let's have the system automatically update all of the individual UI elements that we attached
  //to the serialManager via the setupSerialManager() function used back in the main *.ino file.
  SerialManagerBase::setFullGUIState(activeButtonsOnly); //in here, it automatically loops over the different UI elements

}

#endif
