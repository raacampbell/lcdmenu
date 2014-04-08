
/*                                                                                
 Set up a menu system using two classes: one for the menu system and one for 
 the menu data
*/


// Include the LCD library
#include "LiquidCrystal.h"
#include "LCDmenu.h"



//Define the settings menu
const int settingsMenuLength=7;
Menu settingsMenu[settingsMenuLength] ;

//Define another menu (mainMenu) to link to as an example
const int mainMenuLength=3;
Menu mainMenu[mainMenuLength];


menuDisplay myDisplay(4,20,13,0,1); //lcdRows, lcdCols, thumbStick pin, x pin, y pin


void setup() {

  Serial.begin(9600);          //  setup serial

  //Define the settings menu
  settingsMenu[0].setNumericMenu( 4, 1,  10, 1, "Taste 1 Angle:");
  settingsMenu[1].setNumericMenu(45, 1, 15000, 1, "dAngle:");
  settingsMenu[2].setNumericMenu(80, 2, 178, 0, "Extended pos:");
  settingsMenu[3].setNumericMenu(70, 2, 178, 0, "Retracted pos:");
  settingsMenu[4].setNumericMenu(70, 2, 178, 0, "RETRACTED pos:");
  settingsMenu[5].setActionMenu("To main menu",toMainMenu);
  settingsMenu[6].setActionMenu("Wipe and re-draw",wiper);
  
  //Define the mainMenu
  mainMenu[0].setNumericMenu(4, 1,  10, 1, "Something:");
  mainMenu[1].setNumericMenu(4, 1,  10, 1, "Something ELSE:");
  mainMenu[2].setActionMenu("BACK",toSettingsMenu);


  //Initialise display and set menu
  myDisplay.initDisplay();
  myDisplay.changeMenu(settingsMenu,settingsMenuLength);

}



void loop() {
  //Following variables are "wait-without-delay" counters for listening to the 
  //thumbstick. 
  static long varInterval=25; 
  static unsigned long currentVarMillis;
  static long previousVarMillis=0;

  currentVarMillis=millis();
  if(currentVarMillis - previousVarMillis > varInterval) {  
    previousVarMillis=currentVarMillis; //save update time
        
    myDisplay.poll();
  } //if(currentVarMillis ...


} //function loop







//TEST STUFF BELOW
void wiper(){
  myDisplay.lcd->clear();
  delay(3000);
  myDisplay.refreshScreen();
}


void toMainMenu(){
   myDisplay.changeMenu(mainMenu,mainMenuLength);
   
}

void toSettingsMenu(){
   myDisplay.changeMenu(settingsMenu,settingsMenuLength);
}

