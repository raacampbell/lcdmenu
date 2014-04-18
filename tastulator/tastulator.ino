/*                                                                                
  tastulator.ino

 This sketch runs the taste delivery system. 

 Rob Campbell 2014
*/


#include "LiquidCrystal.h"
#include "LCDmenu.h"

#include <avr/eeprom.h>
// void eeprom_read_block(void *  __dst, const void * __src, size_t __n )
// Read a block of __n bytes from EEPROM address __src to SRAM __dst.



static LiquidCrystal this_lcd(7, 8, 9, 10, 11, 12); //Edit this line for you wiring settings

//Define the LCD size and and the pins the listen to the thumb stick
const int lcdRows=4;  //Number of rows on the LCD character display
const int lcdCols=20; //Number of columns on the LCD character display
const int tStickButton=13;
const int tStickXPin=0;
const int tStickYPin=1;


//Define an instance of the menuDisplay class, which will be responsible for 
//controlling most functions of the menu system
menuDisplay myDisplay(&this_lcd,lcdRows,lcdCols,tStickButton,tStickXPin,tStickYPin);




//Declare two instances of class Menu, which will house the menus that will be displayed
//The first class has 7 rows and the second has three rows. 
const int settingsMenuLength=6; //The number of rows in the menu
Menu settingsMenu[settingsMenuLength];

const int controlMenuLength=3;
Menu controlMenu[controlMenuLength]; //Define another menu to link to as an example

const int statusMenuLength=4;
Menu statusMenu[statusMenuLength]; //Define another menu to link to as an example


short currentTaste=1;
short currentPos=40;


void setup() {

  Serial.begin(9600);

  //Define the settings menu, which is an array of class Menu. The order 
  //of the input arguments is: 
  // 1. displayed value [short] 
  // 2. minimum value [short]
  // 3. maximum value [short]
  // 4. wrap or not [bool]
  // 5. name of row printed to LCD display [String]
  settingsMenu[0].setNumericMenu(80, 2, 178, 0, "Extended pos:");
  settingsMenu[1].setNumericMenu(70, 2, 178, 0, "Retracted pos:");
  settingsMenu[2].setNumericMenu(3,  1,  90, 0, "Taste zero ang:");
  settingsMenu[3].setNumericMenu(30, 5,  45, 1, "dAngle:");
  settingsMenu[4].setNumericMenu(4, 1,  10, 1, "#Tastes:");
  settingsMenu[5].setActionMenu("BACK",toStatusMenu); //executes toMainMenu() when button is pressed

  
  //Define the control menu
  controlMenu[0].setNumericMenu( 4, 1,  10, 1, "Select Taste:");
  controlMenu[1].setNumericMenu(40, 1, 178, 1, "Actuator Pos:");
  controlMenu[2].setActionMenu("BACK",toStatusMenu); //executes toSettingsMenu() when button is pressed
  
  //Define the status menu
  statusMenu[0].setNumericMenu(currentTaste, currentTaste, currentTaste, 1, "Current Taste:");
  statusMenu[1].setNumericMenu(currentTaste, currentTaste, currentTaste, 1, "Current Pos:");
  statusMenu[2].setActionMenu("SETTINGS",toSettingsMenu); //executes toSettingsMenu() when button is pressed
  statusMenu[3].setActionMenu("CONTROL",toControlMenu); //executes toSettingsMenu() when button is pressed

  //Initialise the display and bring up the default menu
  myDisplay.initDisplay();

  /*
  Serial.print("Value: ");
  Serial.println(controlMenu[0].value);
  statusMenu[0].setValue(controlMenu[0].value);
  */

  myDisplay.changeMenu(statusMenu,statusMenuLength);
} //function setup()



void loop() {

  //wait-without-delay counters for listening to the thumbstick. 
  static long varInterval=25; 
  static unsigned long currentVarMillis;
  static long previousVarMillis=0;

  currentVarMillis=millis();
  if (currentVarMillis - previousVarMillis > varInterval) {  
    previousVarMillis=currentVarMillis; //save update time
    myDisplay.poll();
    //Serial.println(myDisplay.getValue());
    //statusMenu[0].setValue(controlMenu[0].value);
    //Update the status menu
    updateFixedMenu(&statusMenu[0],controlMenu[0].value);
    updateFixedMenu(&statusMenu[1],controlMenu[1].value);
  } //if (currentVarMillis 

} //function loop







//------------------------------------------------------------------------------


//The following functions are executed on button press and used to switch menus
void toStatusMenu(){
   myDisplay.changeMenu(statusMenu,statusMenuLength);
   
}
void toSettingsMenu(){
   myDisplay.changeMenu(settingsMenu,settingsMenuLength);
}

void toControlMenu(){
   myDisplay.changeMenu(controlMenu,controlMenuLength);
}


void updateFixedMenu(Menu *thisMenuRow, short val){
  thisMenuRow->setValue(val);
  thisMenuRow->setMax(val);
  thisMenuRow->setMin(val);
  }