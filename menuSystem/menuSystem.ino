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
const int settingsMenuLength=7; //The number of rows in the menu
Menu settingsMenu[settingsMenuLength];

const int mainMenuLength=3;
Menu mainMenu[mainMenuLength]; //Define another menu to link to as an example





void setup() {


  //Define the settings menu, which is an array of class Menu. The order 
  //of the input arguments is: 
  // 1. displayed value [short] 
  // 2. minimum value [short]
  // 3. maximum value [short]
  // 4. wrap or not [bool]
  // 5. name of row printed to LCD display [String]
  settingsMenu[0].setNumericMenu( 4, 1,  10, 1, "A small number:");
  settingsMenu[1].setNumericMenu(45, 1,1000, 1, "A big number:");
  settingsMenu[2].setNumericMenu(80, 2, 178, 0, "Extended pos:");
  settingsMenu[3].setNumericMenu(70, 2, 178, 0, "Retracted pos:");
  settingsMenu[4].setNumericMenu(70, 2, 178, 0, "Turnips:");
  settingsMenu[5].setActionMenu("To main menu",toMainMenu); //executes toMainMenu() when button is pressed
  settingsMenu[6].setActionMenu("Wipe and re-draw",wiper);  //executes wiper() when button is pressed
  
  //Define the mainMenu
  mainMenu[0].setNumericMenu(4, 1,  10, 1, "Poplar:");
  mainMenu[1].setNumericMenu(4, 1,  10, 1, "Willow:");
  mainMenu[2].setActionMenu("BACK",toSettingsMenu); //executes toSettingsMenu() when button is pressed



  //Initialise the display and bring up the default menu
  myDisplay.initDisplay();
  myDisplay.changeMenu(settingsMenu,settingsMenuLength);

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
  } //if (currentVarMillis 

} //function loop







//------------------------------------------------------------------------------

//This example function wipes the screen, pauses, then re-draws the screen. 
//It is executed on a button press when the appropriate menu row is highlighted
void wiper(){
  myDisplay.lcd->clear();
  delay(3000);
  myDisplay.refreshScreen();
}


//The following functions are executed on button press and used to switch menus
void toMainMenu(){
   myDisplay.changeMenu(mainMenu,mainMenuLength);
   
}

void toSettingsMenu(){
   myDisplay.changeMenu(settingsMenu,settingsMenuLength);
}

