
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
extern AudioEffectBTNRH BTNRH_alg1, BTNRH_alg2;

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

    void printFeedbackCoeff(AudioEffectBTNRH &alg);
      
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
  Serial.println(" Static Info: ");
  Serial.println("      : BTNRH_alg1 setup complete = " + String(BTNRH_alg1.setup_complete) + ", BTNRH_alg2 setup complete = " + String(BTNRH_alg2.setup_complete));
  Serial.println(" AFC: (no prefix)");
  Serial.println("   m/M: incr/decrease mu (current: " + String((float)(BTNRH_alg1.get_cha_dvar(_mu)),8) + ")");
  Serial.println("   r/R: incr/decrease rho (current: " + String((float)(BTNRH_alg1.get_cha_dvar(_rho)),8) + ")");
  Serial.println("   e/E: incr/decrease eps (current: " + String((float)(BTNRH_alg1.get_cha_dvar(_eps)),8) + ")");
  Serial.println("   s/S: print left or right AFC settings.");
  Serial.println("   q/Q: reset left or right feedback model.");
  Serial.println("   f/F: print left or right feedback model.  Prints ONCE.");
  Serial.println("   d/D: start/stop REPEATED printing of LEFT feedback model.");
  Serial.println("   g/G: start/stop REPEATED printing of RIGHT feedback model.");

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
  int ind;
  double old_val, new_val, scale_fac=1.0;
  
  switch (c) {
    case 'h':
      printHelp(); 
      break;
    case 'm':
      ind = _mu; scale_fac = 2.0f;
      old_val = BTNRH_alg1.get_cha_dvar(ind); new_val = old_val *= scale_fac;
      BTNRH_alg1.set_cha_dvar(ind, new_val);  BTNRH_alg2.set_cha_dvar(ind, new_val);
      BTNRH_alg1.set_cha_ivar(_in1, 0); BTNRH_alg2.set_cha_ivar(_in1, 0);  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: changing AFC mu to "); myTympan.println(BTNRH_alg1.get_cha_dvar(ind),7);
      break;
    case 'M':
      ind = _mu; scale_fac = 1.0/2.0f;
      old_val = BTNRH_alg1.get_cha_dvar(ind); new_val = old_val *= scale_fac;
      BTNRH_alg1.set_cha_dvar(ind, new_val); BTNRH_alg2.set_cha_dvar(ind, new_val);
      BTNRH_alg1.set_cha_ivar(_in1, 0); BTNRH_alg2.set_cha_ivar(_in1, 0);  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: changing AFC mu to "); myTympan.println(BTNRH_alg1.get_cha_dvar(ind),7);
      break;
    case 'r':
      ind = _rho; old_val = BTNRH_alg1.get_cha_dvar(ind); new_val = 1.0-((1.0-old_val)/sqrtf(2.0));
      BTNRH_alg1.set_cha_dvar(ind, new_val);  BTNRH_alg2.set_cha_dvar(ind, new_val);
      BTNRH_alg1.set_cha_ivar(_in1, 0); BTNRH_alg2.set_cha_ivar(_in1, 0);  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: changing AFC rho to "); myTympan.println(BTNRH_alg1.get_cha_dvar(ind),7);
      break;
    case 'R':
      ind = _rho; old_val = BTNRH_alg1.get_cha_dvar(ind); new_val = 1.0-((1.0-old_val)*sqrtf(2.0));
      BTNRH_alg1.set_cha_dvar(ind, new_val);  BTNRH_alg2.set_cha_dvar(ind, new_val);
      BTNRH_alg1.set_cha_ivar(_in1, 0); BTNRH_alg2.set_cha_ivar(_in1, 0);  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: changing AFC rho to "); myTympan.println(BTNRH_alg1.get_cha_dvar(ind),7);
      break;
    case 'e':
      ind = _eps; scale_fac = sqrtf(10.0f);
      BTNRH_alg1.set_cha_dvar(ind, new_val);  BTNRH_alg2.set_cha_dvar(ind, new_val);
      BTNRH_alg1.set_cha_ivar(_in1, 0); BTNRH_alg2.set_cha_ivar(_in1, 0);  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: changing AFC eps to "); myTympan.println(BTNRH_alg1.get_cha_dvar(ind),7);
      break;
    case 'E':
      ind = _eps; scale_fac = 1.0/sqrtf(10.0f);
      BTNRH_alg1.set_cha_dvar(ind, new_val);  BTNRH_alg2.set_cha_dvar(ind, new_val);
      BTNRH_alg1.set_cha_ivar(_in1, 0); BTNRH_alg2.set_cha_ivar(_in1, 0);  //command afc_process to re-initialize its parameters
      myTympan.print("Command received: changing AFC eps to "); myTympan.println(BTNRH_alg1.get_cha_dvar(ind),7);
      break;
    case 'q':
      Serial.println("SerialManager: command received...reseting LEFT AFC feedback model...");
      BTNRH_alg1.reset_feedback_model();
      break;
    case 'Q':
      Serial.println("SerialManager: command received...reseting RIGHT AFC feedback model...");
      BTNRH_alg2.reset_feedback_model();
      break;    
    case 's':
      Serial.println("SerialManager: command received...print settings for LEFT AFC:");
      BTNRH_alg1.print_afc_params();
      break;
    case 'S':
      Serial.println("SerialManager: command received...print settings for RIGHT AFC:");
      BTNRH_alg2.print_afc_params();
      break;
    case 'f':
      Serial.println("SerialManager: command received...feedback model for LEFT channel:");
      printFeedbackCoeff(BTNRH_alg1);
      break;
    case 'F':
      Serial.println("SerialManager: command received...feedback model for RIGHT channel:");
      printFeedbackCoeff(BTNRH_alg2);
      break;
    case 'd':
      Serial.println("SerialManager: START printing feedback model for LEFT channel...");
      myState.flag_printLeftFeedbackModel = true;
      break;
    case 'D':
      Serial.println("SerialManager: STOP printing feedback model for LEFT channel...");
      myState.flag_printLeftFeedbackModel = false;
      break;
    case 'g':
      Serial.println("SerialManager: START printing feedback model for LEFT channel...");
      myState.flag_printRightFeedbackModel = true;
      break;
    case 'G':
      Serial.println("SerialManager: STOP printing feedback model for LEFT channel...");
      myState.flag_printRightFeedbackModel = false;
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

void SerialManager::printFeedbackCoeff(AudioEffectBTNRH &alg) {
  int n_coeff = alg.get_cha_ivar(_afl);
  float *efbp = (float *)(alg.cp[_efbp]);
  for (int i=0; i<n_coeff; i++) { 
    Serial.print(efbp[i],6); 
    if (i < n_coeff-1) Serial.print(", ");
  }
  Serial.println();
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
