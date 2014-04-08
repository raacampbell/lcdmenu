/*
Purpose of this file is to off-load settings and stuff like that to keep the main program neat
*/

//arrowPos is the line at which to draw the arrow in the first column of the LCD
short arrowPos=0;

//This is the row of the menu currently on row zero of the LCD
byte currentTopRow=0;

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);



//Stick position paramaters
const short deadZone=20; //Make sure the stick is moved a good amount before ticks are added to the counter
const short centre=515; //Value of stick in the centre


//The pin to which the thumbstick button is connected
const int buttonPin=13;




//The number of rows and columns on the display. 
const byte lcdRows=4;
const byte lcdCols=20;



//Define up and down arrow symbols for indicating when there
//are more menu items further up or down the page. 
byte upArrow[8] = {
  B00100,
  B01010,
  B10001,
  B00100,
  B01010,
  B10001,
  B00000,
};

byte downArrow[8] = {
  B00000,
  B10001,
  B01010,
  B00100,
  B10001,
  B01010,
  B00100,
};


