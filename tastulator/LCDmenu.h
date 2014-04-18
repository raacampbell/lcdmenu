/*----------------------------------------------------------------------*\
 LCDmenu 
 Rob Campbell - 2014
 GPL

 LCDmenu is a library for building an interactive menu system using an
 Arduino, a character display, and a thumbstick. 
 
 class menuDisplay sets up the LCD display and interfaces with the 
 thumbstick. 

 class Menu stores the data for the menus and coordinates how displayed
 values are incremented and how they behave. 
 
\*----------------------------------------------------------------------*/



#ifndef LCDmenu_h
#define LCDmenu_h

#include "Arduino.h"
#include "LiquidCrystal.h"


//----------------------------------------------------------------------
class Menu
{
  public:
    bool isVariable;
    short value; //Value displayed on screen.
    String menuString();
    void (*buttonFunction) (); //Used to execute any arbitrary function that returns void
    
    Menu();
    void setNumericMenu(short val,short minVal,short maxVal,bool wrap,String menuStr);
    void setActionMenu(String menuStr, void (*funct)() ); //Can link to any function that returns void
    short incrementVal(short incrementBy);
    void setVars(byte lcdRows, byte lcdCols);
    byte menuStringLength();

  private:
    byte _lcdRows;  //character rows on the LCD display
    byte _lcdCols;  //character columns on the LCD display
    short _minVal; //minimum allowable value 
    short _maxVal; //maximum allowable value
    bool _wrap;   //if 1 we wrap from min to max value
    String _menuString; //The menu string itself
};







//----------------------------------------------------------------------
class menuDisplay
{
  public:    
  Menu* currentMenu;
  int nMenuRows;
  byte currentMenuRow;
  
  menuDisplay(LiquidCrystal* thisLCD, byte lcdRows, byte lcdCols, int buttonPin, int xLine, int yLine); 
  void initDisplay();
  void changeMenu(Menu newMenu[], int nRows);    
  long poll();
  void refreshScreen();
  static LiquidCrystal* lcd;   

  private:
  byte _lcdRows;  //character rows on the LCD display
  byte _lcdCols;  //character columns on the LCD display




  short _arrowPos;     //Line at which to draw the arrow in the first column of the LCD
  byte _currentTopRow; //Row of the menu currently on row zero of the LCD
  

  //Thumbstick position paramaters
  short _deadZone; //Make sure the stick is moved a good amount before ticks are added to the counter
  short _centre;   //Value of stick in the centre


  //Variables to hold the up and down arrow symbols for indicating when there
  //are more menu items further up or down the page. 
  static byte _upArrow[8];
  static byte _downArrow[8];
  

  unsigned short _numIncrements; //number of continuous sequential increments of a menu variable 
  bool _lastIncremented;        //Was the variable incremented in the last pass through the loop?
  short _incrementBy;             //How much to add to the value being incremented

  //Lines connected to the thumbstick
  int _buttonPin;
  int _xLine;
  int _yLine;
  
  //Private methods
  //LiquidCrystal connectLCD(int A, int B, int C, int D, int E, int F); //TO DO IN FUTURE
  void updateCursor();
  void updateRowValue(byte menuRow);
  void printArrow();


};


#endif
