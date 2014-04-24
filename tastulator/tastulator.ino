/*                                                                                
  tastulator.ino

 This sketch runs the taste delivery controller based upon a menu system. 

 Rob Campbell 2014
*/


#include "LiquidCrystal.h"
#include "LCDmenu.h"
#include <avr/eeprom.h> //http://playground.arduino.cc/Code/EEPROMWriteAnything
#include <Servo.h> 


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
const int settingsMenuLength=9; //The number of rows in the menu
Menu settingsMenu[settingsMenuLength];

const int controlMenuLength=4;
Menu controlMenu[controlMenuLength]; //Define another menu to link to as an example

const int statusMenuLength=4;
Menu statusMenu[statusMenuLength]; //Define another menu to link to as an example


//We will have the ability to load the values of settingsMenu from the EEPROM
const byte lengthValues=6;
short values[lengthValues]={80,70,3,30,4,0}; //Default values. 

//Create pointers to menu entries we will need to access at run-time
Menu* pZERO_ANG   = &settingsMenu[2];
Menu* pDELTA_ANG  = &settingsMenu[3];
Menu* pNTASTES  = &settingsMenu[4];
Menu* pNIDAQ_IN = &settingsMenu[5];
Menu* pSAVE = &settingsMenu[6];
Menu* pLOAD = &settingsMenu[7];


Menu* pSELECT_TASTE = &controlMenu[0]; 
Menu* pSERVO_ANG    = &controlMenu[1]; 
Menu* pACTUATOR_POS = &controlMenu[2]; 

Menu* pCUR_TASTE = &statusMenu[0]; 
Menu* pCUR_ANGLE = &statusMenu[1];
//Menu* pCUR_POS   = &statusMenu[2];



//Motor pins
const byte servoPin=3;
const byte actuatorPin=5;
Servo myServo;  

//Binary inputs for control via a NIDAQ 
const byte IN1=6; //Binary input 1
const byte IN2=4; //Binary input 2
const byte ACT=2; //Extend/Retract 

//These will need to be dealt with
short currentTaste=1;
short currentPos=40;



void setup() {

  Serial.begin(9600);
  myServo.attach(servoPin); 
  pinMode(actuatorPin,OUTPUT);

  //Set up binary inputs we will use to read from NIDAQ
  //[do we need pullup/pulldown?]
  pinMode(IN1,INPUT);
  pinMode(IN2,INPUT);
  pinMode(ACT,INPUT);


  //Define the settings menu, which is an array of class Menu. The order 
  //of the input arguments is: 
  // 1. displayed value [short] 
  // 2. minimum value [short]
  // 3. maximum value [short]
  // 4. wrap or not [bool]
  // 5. name of row printed to LCD display [String]
  settingsMenu[0].setNumericMenu(values[0], 2, 178, 0, "Extended pos:");
  settingsMenu[1].setNumericMenu(values[1], 2, 178, 0, "Retracted pos:");
  settingsMenu[2].setNumericMenu(values[2],  1, 90, 0, "Taste zero ang:");
  settingsMenu[3].setNumericMenu(values[3], 5,  45, 1, "dAngle:");
  settingsMenu[4].setNumericMenu(values[4], 1,  10, 1, "#Tastes:");
  settingsMenu[5].setNumericMenu(values[5], 0,   1, 0, "NIDAQ in [0/1]:");
  settingsMenu[6].setActionMenu("SAVE",save);
  settingsMenu[7].setActionMenu("LOAD",load);
  settingsMenu[8].setActionMenu("BACK",toStatusMenu); //executes toMainMenu() when button is pressed

  
  //Define the control menu
  controlMenu[0].setNumericMenu( 4, 1,  10, 0, "Select Taste:");
  controlMenu[1].setNumericMenu( 4, 3,  150, 0, "Servo angle:");
  controlMenu[2].setNumericMenu(40, 1, 178, 1, "Actuator Pos:");
  controlMenu[3].setActionMenu("BACK",toStatusMenu); //executes toSettingsMenu() when button is pressed
  
  //Define the status menu
  statusMenu[0].setNumericMenu(currentTaste, currentTaste, currentTaste, 1, "Current Taste:");
  statusMenu[1].setNumericMenu(currentTaste, currentTaste, currentTaste, 1, "Current Pos:");
  statusMenu[2].setActionMenu("SETTINGS",toSettingsMenu); //executes toSettingsMenu() when button is pressed
  statusMenu[3].setActionMenu("CONTROL",toControlMenu); //executes toSettingsMenu() when button is pressed

  //Initialise the display and bring up the default menu
  myDisplay.initDisplay();
  myDisplay.changeMenu(statusMenu,statusMenuLength);

} //function setup()



void loop() {
  static short lastVal=myDisplay.getValue(); //The last value of the current line

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
      lastVal=myDisplay.getValue();
      updateSAVE();
      myDisplay.refreshScreen();
    }
    //Update the status menu
    updateFixedMenu(pCUR_TASTE,pSELECT_TASTE->value);
    updateFixedMenu(pCUR_ANGLE,pSERVO_ANG->value);

     
  } //if (currentVarMillis 


  //If the user modifies the control settings for current taste then we need to act on this
  //Some settings potentially conflict (e.g. select taste and servo angle) so we want the
  //most recently modified number to take precedence and to modify the other number. 
  static short controlTaste = pSELECT_TASTE->value;
  static short controlAngle = pSERVO_ANG->value;
  static short actuatorPos  = pACTUATOR_POS->value;

  if (controlTaste != pSELECT_TASTE->value){
    pSERVO_ANG->setValue(taste2angle(pSELECT_TASTE->value));
    controlTaste=pSELECT_TASTE->value;
    controlAngle = pSERVO_ANG->value;
    myDisplay.refreshScreen();
    myServo.write(pSERVO_ANG->value); 
  }

 if (controlAngle != pSERVO_ANG->value){
    pSELECT_TASTE->setValue(angle2taste(pSERVO_ANG->value));
    controlAngle = pSERVO_ANG->value;
    controlTaste = pSELECT_TASTE->value;
    myDisplay.refreshScreen();
    myServo.write(pSERVO_ANG->value); 
  }

 if (actuatorPos != pACTUATOR_POS->value){
  analogWrite(actuatorPin,actuatorPos);
  actuatorPos = pACTUATOR_POS->value;
 }
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
  //#tastes in the settings menu specifies the maximum number of tastes
  //So let's makes sure that the control menus selector, which allows us
  //to choose a taste, honours this maximum value
  controlMenu[0].setMax(settingsMenu[4].value);
  myDisplay.changeMenu(controlMenu,controlMenuLength);
}

//Sets value, max, and min values to the same number. This is used to construct a 
//a menu row that is not modifiable
void updateFixedMenu(Menu *thisMenuRow, short val){
  thisMenuRow->setValue(val);
  thisMenuRow->setMax(val);
  thisMenuRow->setMin(val);
  }



//Save settings to EEPROM
void save(){
 for (byte ii=0; ii<5; ii++){
    values[ii]=settingsMenu[ii].value;
  }
  eeprom_write_block((const void*)&values, (void*)0, sizeof(values));
  pSAVE->setMenuString("SAVE"); //In case there was an asterisk
  myDisplay.refreshScreen();
}

//Load settings from EEPROM
void load(){
 // void eeprom_read_block(void *  __dst, const void * __src, size_t __n )
 // Read a block of __n bytes from EEPROM address __src to SRAM __dst.
 eeprom_read_block((void*)&values, (void*)0, sizeof(values));
  for (byte ii=0; ii<lengthValues; ii++){
    settingsMenu[ii].setValue(values[ii]);
  }
  
  //Provide a visual indicator that we've loaded the menu
  //(side effect of refreshing screen with new values)
  //settingsMenu[loadIndex].setMenuString("");
  pLOAD->setMenuString("");
  pSAVE->setMenuString("SAVE"); //In case there was an asterisk
  myDisplay.refreshScreen();
  delay(250);
  pLOAD->setMenuString("LOAD");
  myDisplay.refreshScreen();

}



//Update the "SAVE" menu row to add an asterisk when any any saved setting has been changed. 
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
    pSAVE->setMenuString("SAVE*");
  } else {
    pSAVE->setMenuString("SAVE");
  }

}  


//Functions to convert back and forth between tastes and angles
short angle2taste(short angle){
  short out = round((angle-pZERO_ANG->value)/pDELTA_ANG->value)+1;
  if (out>pNTASTES->value){
    out=pNTASTES->value;
  }
  return out;
}

short taste2angle(short taste){
  return (taste*pDELTA_ANG->value)+pZERO_ANG->value;

}