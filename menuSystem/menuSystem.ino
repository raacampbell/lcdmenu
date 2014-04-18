/*                                                                                
  menuSystem.ino

 This is an example of how to build an interactive menu system using an Arduino,
 a thumbstick, and a LCD character display.

 Each row displays a variable name as a string and a value as an integer. 
 The user selects the desired row by pushing the stick up or down and 
 sets the value of the integer by pushing the stick left or right. The user
 builds the menu system using a class. The user sets an initial value, max, 
 and min for each row. In addition, the user defines if the values should wrap 
 around. The display increments by 10s, 100s, etc, automatically as needed. 

 A menue row can also be linked a function that returns void. Such rows do not have 
 a numeric value associated with them. This allows the user to do things such as
 build linked lists of menus, or execute other arbitrary commands via the menu
 system. 

 Rob Campbell 2014
*/


#include "LiquidCrystal.h"
#include "LCDmenu.h"
#include <avr/eeprom.h> //http://playground.arduino.cc/Code/EEPROMWriteAnything


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
//The first class has 9 rows and the second has three rows. 
const int settingsMenuLength=9; //The number of rows in the menu
Menu settingsMenu[settingsMenuLength];
byte saveIndex=6; //Index of row containing "SAVE" menu
byte loadIndex=7; //Index of row containing "LOAD" menu

//Define another menu to link to. This will contain some entries that just
//reflect values chosen in settingsMenu
const int statusMenuLength=4;
Menu statusMenu[statusMenuLength]; 


//We will have the ability to load the values of settingsMenu from the EEPROM
const byte lengthValues=5;
short values[lengthValues]={0,0,0,0,0};




void setup() {

  Serial.begin(115200);
  
  //Define the settings menu, which is an array of class Menu. The order 
  //of the input arguments is: 
  // 1. displayed value [short] 
  // 2. minimum value [short]
  // 3. maximum value [short]
  // 4. wrap or not [bool]
  // 5. name of row printed to LCD display [String]
  settingsMenu[0].setNumericMenu(values[0], 1,  10, 1, "Setting A:");
  settingsMenu[1].setNumericMenu(values[1], 1,1000, 1, "Setting B:");
  settingsMenu[2].setNumericMenu(values[2], 2, 178, 0, "Extended pos:");
  settingsMenu[3].setNumericMenu(values[3], 2, 178, 0, "Retracted pos:");
  settingsMenu[4].setNumericMenu(values[4], 2, 178, 0, "Turnips:");
  settingsMenu[5].setActionMenu("To status menu",toStatusMenu); //executes toMainMenu() when button is pressed
  settingsMenu[saveIndex].setActionMenu("SAVE",save);
  settingsMenu[loadIndex].setActionMenu("LOAD",load);
  settingsMenu[8].setActionMenu("Wipe and re-draw",wiper);  //executes wiper() when button is pressed


  //Define the statusMenu
  //The first two rows are automically set to what the user selected in the
  //settings menu. These numbers can't be modified from within this menu. The
  //code that performs this is located in this function, not in the library. 
  statusMenu[0].setNumericMenu(0, 0, 0, 1, "A:");
  statusMenu[1].setNumericMenu(0, 0, 0, 1, "B:");
  statusMenu[2].setNumericMenu(79, 0, 99, 1, "This changes:");
  statusMenu[3].setActionMenu("BACK",toSettingsMenu); //executes toSettingsMenu() when button is pressed



  //Initialise the display and bring up the default menu
  myDisplay.initDisplay();
  myDisplay.changeMenu(settingsMenu,settingsMenuLength);

} //function setup()



void loop() {
  static short lastVal=myDisplay.getValue(); 

  //wait-without-delay counters for listening to the thumbstick. 
  static long varInterval=25; 
  static unsigned long currentVarMillis;
  static long previousVarMillis=0;

  currentVarMillis=millis();
  if (currentVarMillis - previousVarMillis > varInterval) {  
    previousVarMillis=currentVarMillis; //save update time
    myDisplay.poll();
    
    
    //Perform following actions only on value-change
    if (lastVal != myDisplay.getValue()){
      //Write to serial port
      Serial.println(myDisplay.getValue());
      lastVal=myDisplay.getValue();
      //Add an asterisk to the SAVE menu if a value has been changed so it differs from the
      //stored values 
      updateSAVE();
      myDisplay.refreshScreen();
    }


    //Keep the fixed values in status menu updated
    updateFixedMenu(&statusMenu[0],settingsMenu[0].value);
    updateFixedMenu(&statusMenu[1],settingsMenu[1].value);

  } //if (currentVarMillis 

  //Display current value to serial terminal when it's changed
  
} //function loop









//------------------------------------------------------------------------------

//This example function wipes the screen, pauses, then re-draws the screen. 
//It is executed on a button press when the appropriate menu row is highlighted
void wiper(){
  myDisplay.lcd->clear();
  delay(1000);
  myDisplay.refreshScreen();
}


//The following functions are executed on button press and used to switch menus
void toStatusMenu(){
   myDisplay.changeMenu(statusMenu,statusMenuLength);
   
}


void updateFixedMenu(Menu *thisMenuRow, short val){
  thisMenuRow->setValue(val);
  thisMenuRow->setMax(val);
  thisMenuRow->setMin(val);
  }

void toSettingsMenu(){
   myDisplay.changeMenu(settingsMenu,settingsMenuLength);
}

void save(){
 for (byte ii=0; ii<5; ii++){
    values[ii]=settingsMenu[ii].value;
  }
  eeprom_write_block((const void*)&values, (void*)0, sizeof(values));
  settingsMenu[saveIndex].setMenuString("SAVE"); //In case there was an asterisk
  myDisplay.refreshScreen();
}


void load(){
  eeprom_read_block((void*)&values, (void*)0, sizeof(values));
  for (byte ii=0; ii<5; ii++){
    settingsMenu[ii].setValue(values[ii]);
  }
  
  //Provide a visual indicator that we've loaded the menu
  //(side effect of refreshing screen with new values)
  settingsMenu[loadIndex].setMenuString("");
  settingsMenu[saveIndex].setMenuString("SAVE"); //In case there was an asterisk
  myDisplay.refreshScreen();
  delay(400);
  settingsMenu[loadIndex].setMenuString("LOAD");
  myDisplay.refreshScreen();

}


void updateSAVE(){

  //Assumes all values are at start of settingsMenu
  byte diffs=0;
  for (byte ii=0; ii<lengthValues; ii++){
    if (settingsMenu[ii].value != values[ii]){
      diffs++;
      break;
    }
  }
  
  if (diffs){
    settingsMenu[saveIndex].setMenuString("SAVE*");
  } else {
    settingsMenu[saveIndex].setMenuString("SAVE");
  }

}