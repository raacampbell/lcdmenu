
/*
  Menu.cpp - Library for building LCD menus
*/

#include "LCDmenu.h"


LiquidCrystal* menuDisplay::lcd ; //If this isn't here, the display doesn't work for some reason

//Initialise static const arrays for the special characters
//http://stackoverflow.com/questions/11367141/how-to-declare-and-initialize-a-static-const-array-as-a-class-member
 byte menuDisplay::_upArrow[8] = 
  {
    B00100,
    B01010,
    B10001,
    B00100,
    B01010,
    B10001,
    B00000};

 byte menuDisplay::_downArrow[8] = 
  {
    B00000,
    B10001,
    B01010,
    B00100,
    B10001,
    B01010,
    B00100};



//Constructor
menuDisplay::menuDisplay(LiquidCrystal* thisLCD, byte lcdRows, byte lcdCols, int buttonPin, int xLine, int yLine)
{
  _lcdRows=lcdRows;
  _lcdCols=lcdCols;
  _buttonPin=buttonPin;
  _xLine=xLine;
  _yLine=yLine;

  _arrowPos=0; //arrowPos is the line at which to draw the arrow in the first column of the LCD
  _currentTopRow=0; //This is the row of the menu currently on row zero of the LCD
  
  _deadZone=20; //Make sure the stick is moved a good amount before ticks are added to the counter
  _centre=515; //Value of stick in the centre

  //The following variables are used to tell for how long the 
  //user has been pushing at the stick. This allows us to 
  //increment numbers by 10x or 100x, as desired. 
  _numIncrements=0; //The number of continuous increments 
  _lastIncremented=0; //Was the variable incremented in the last pass?
  _incrementBy=1;

  lcd=thisLCD;
}


//Initialise the display 
void menuDisplay::initDisplay()
{

  pinMode(_buttonPin,INPUT); 

  lcd->begin(_lcdCols, _lcdRows);
  lcd->createChar(0, _upArrow);
  lcd->createChar(1, _downArrow);  
  lcd->setCursor(0,0);
  refreshScreen();

}


//Change the on-screen menu, refresh the screen
void menuDisplay::changeMenu(Menu newMenu[], int nRows){
  currentMenu=newMenu;
  nMenuRows=nRows;
  _currentTopRow=0;
  _arrowPos=0;

  //update variables within newMenu
  for (int ii=0; ii<nRows; ii++){
    newMenu[ii].setVars(_lcdRows, _lcdCols);
  }
  refreshScreen();
}


//Poll the stick position and update screen appropriately. 
//This is the main workhorse function
long menuDisplay::poll(){
 
  //Read the X and Y directions of the stick
  short x=analogRead(_xLine);
  short y=analogRead(_yLine);

  short delta_y=y-_centre;  
  short delta_x=x-_centre;  
   
  //How long to wait before leaving function. This number determines 
  //how quickly digits change and the arrow progresses
  float waitTime=0; 

  //Move the little arrow indicating the line we're on. Scroll
  //screen if appropriate. 
  if (abs(delta_y)>_deadZone){
   //wait and re-read to avoid reading values before thumbstick has settled
    delay(20);
    short y=analogRead(_yLine);
    delta_y=y-_centre;  

   if (y>_centre){
     _arrowPos--;
    } else if (y<_centre){
     _arrowPos++;
    }
   updateCursor();

   //Wait before updating
   waitTime = _centre-abs(delta_y);
   waitTime = 150 + (waitTime / (2.0 * _centre)) * 1000.0; //Never faster than 100ms

   _currentMenuRow = _arrowPos + _currentTopRow; //Keep track of the currently selected row
  }//if (abs(delta_y)...


  //Allow updates of the values on the current row
  byte menuRow=_arrowPos+_currentTopRow; //The menu row (allows for scrolling)

  //If we've cycled through at least 4 numbers squentially and we're on a 
  //multiple of the next increment rate then increase increment rate to this new rate
  if (abs(delta_x)>_deadZone)
  {

    if (_numIncrements>=4){
      if ( (currentMenu[menuRow].value % (_incrementBy*10) )==0){
        _numIncrements=1;
        _incrementBy *= 10; 
      }     
    }


    if (x>_centre)
    {
     short incrementBy=currentMenu[menuRow].incrementVal(-1 * _incrementBy);
     if (incrementBy != _incrementBy){ //Then the numbers wrapped and we want to reset the incrementing
      _incrementBy=incrementBy;
      _numIncrements=1;
     }
    } else if (x<_centre){
     short incrementBy=currentMenu[menuRow].incrementVal(_incrementBy);
     if (incrementBy != _incrementBy){ //Then the numbers wrapped and we want to reset the incrementing
      _incrementBy=incrementBy;
      _numIncrements=1;
     }
    }
 
    //Only updating the screen if the value has changed is neater. The value will not "flash"
    if (currentMenu[menuRow].value != currentMenu[menuRow].lastValue){
      updateRowValue(menuRow);
    } 
      

    //Apply wait
    waitTime = _centre-abs(delta_x);
    waitTime = 150 + (waitTime / (2.0 * _centre)) * 1000.0; //Never faster than 100ms
     
    if (_lastIncremented){
     _numIncrements++;
    }
   
    _lastIncremented=1;

  } else {

    _lastIncremented=0;
    _numIncrements=0;
    _incrementBy=1;

  }//if (abs(delta_x)...

  //Finally, check if button is pressed and, if so, run the appropriate function if this is possible
  if (digitalRead(_buttonPin)==HIGH){
    if (!currentMenu[menuRow].isVariable){
      currentMenu[menuRow].buttonFunction();
    }
  } 

  
  if (waitTime>0){
    long currentMillis = millis();
    while (millis()-currentMillis < waitTime){}
  }

  return waitTime;


}




//re-draw the whole screen
void menuDisplay::refreshScreen(){

 byte screenRow=0;
 for (byte menuRow=_currentTopRow; menuRow<_currentTopRow+_lcdRows; menuRow++){
   if (screenRow>=nMenuRows){
     lcd->setCursor(0,screenRow++);
    for (int ii=0; ii<_lcdCols; ii++){  lcd->print(" "); } //wipe whole line
   } else {
    lcd->setCursor(1,screenRow++);
    lcd->print(currentMenu[menuRow].menuString());
    updateRowValue(menuRow);
   } // if (screenRow>nMenuRows)
   
 }


 updateCursor(); //The highlighted line
 
 //Add up or down arrows depending on where we are in the
 //menu system
 lcd->setCursor(_lcdCols-1,0); lcd->print(" ");
 lcd->setCursor(_lcdCols-1,_lcdRows-1); lcd->print(" ");

 if (_currentTopRow>0){
   lcd->setCursor(_lcdCols-1,0);
   lcd->write(byte(0));
 }
 if ((_currentTopRow+_lcdRows)<nMenuRows){
   lcd->setCursor(_lcdCols-1,_lcdRows-1);
   lcd->write(byte(1));
 }

} //void refreshScreen



//update the little arrow (cursor) as the user pushes the thumbstick
void menuDisplay::updateCursor(){
 byte lastRow;

 if (nMenuRows>_lcdRows){
   lastRow=_lcdRows;
 } else {
   lastRow=nMenuRows;
 }

 //clear arrow position row
 for (byte ii=0; ii<_lcdRows; ii++){
  lcd->setCursor(0,ii);
  lcd->print(" ");
 }

 //If arrow is within bounds we print it
 if (_arrowPos>=0 && _arrowPos<lastRow){
   printArrow();
   return;
 } 

 //If arrow has moved beyond the lowest menu row but this isn't the lowest
 //lcd row, then don't allow arrow beyond this point.  
 if (_arrowPos>=lastRow && lastRow<_lcdRows){
   _arrowPos=lastRow-1;
   printArrow();
   return;
 } 


 //If arrow has gone beyond the top row, we keep it at top row
 if (_arrowPos < 0){
   _arrowPos=0;
   printArrow();
   if (_currentTopRow>0){ //If scroll upwards if required
      _currentTopRow--;
      refreshScreen();
   }

   return;
 }


 //If arrow has gone beyond the bottom lcd row, we keep it at bottom row
 if (_arrowPos>=_lcdRows){
   _arrowPos=_lcdRows-1;
   printArrow();   
   if (nMenuRows-_lcdRows-_currentTopRow > 0){ //If there are more rows to go we scroll down
     _currentTopRow++;
     refreshScreen();   
   }

   return;
 }

   
} //void updateCursor


void menuDisplay::updateRowValue(byte menuRow){
   
   //Wipe the region of the row where the value will be placed
   //byte cursorPos=currentMenu[menuRow].menuStringLength+1;
   byte cursorPos=currentMenu[menuRow].menuStringLength();
   byte screenRow=menuRow-_currentTopRow;
   short val=currentMenu[menuRow].value;

  if (currentMenu[menuRow].isVariable){ //Replace value
     lcd->setCursor(cursorPos,screenRow); 
     lcd->print(val);  
     //Make sure cursor starts AFTER the end of the displayed value
     if (val==0){
      val++;
     }
     cursorPos+=ceil(log10(abs(val)+1)); 
     if (val<0){
      cursorPos++;
     }
  }
 
  //Wipe what's left of the row 
  for (byte ii=cursorPos; ii<=_lcdCols-2; ii++){
    lcd->setCursor(ii,screenRow); 
    lcd->print(" ");  
  }
 

} //void updateRowValue




//void menuDisplay::printArrow(byte screenRow){
void menuDisplay::printArrow(){
   lcd->setCursor(0,_arrowPos);
   lcd->print(">");
}


short menuDisplay::getValue(){
  return currentMenu[_currentMenuRow].value;
}






//----------------------------------------------------------------------
// CLASS MENU FOLLOWS
Menu::Menu(){
}


void Menu::setNumericMenu(short val,short minVal,short maxVal,bool wrap,String menuStr)
{
	isVariable=1;
	value=val;
	_menuString=menuStr;
	_maxVal=maxVal;
	_minVal=minVal;
	_wrap=wrap;
}

void Menu::setActionMenu(String menuStr, void (*funct)() )
{
	isVariable=0;
	_menuString=menuStr;
	buttonFunction=funct;
}

void Menu::setVars(byte lcdRows, byte lcdCols )
{
  _lcdRows=lcdRows;
  _lcdCols=lcdCols;
}


/**
Chops off the end of the menu string if it's too long given the number of characters
taken up by the value. Displaying the value takes priority over displaying the menustring 
*/
String Menu::menuString(){
  byte usableCols=_lcdCols-2; 

  //Find the maximum number of digits (including minus sign for negative numbers)
  short maxVal=_maxVal;
  short minVal=_minVal;

  if (maxVal==0){ maxVal++; } //cope with zero
  if (minVal==0){ minVal++; } //cope with zero

  byte maxChars=ceil(log10(abs(minVal)+1));
  byte minChars=ceil(log10(abs(maxVal)+1));

  if (_maxVal<0){ maxChars++; }
  if (_minVal<0){ minChars++; }

  byte maxDigits;
  if (maxChars>minChars){
    maxDigits=maxChars;
  } else {
    maxDigits=minChars;
  }

  int maxStringLength = usableCols - maxDigits-1;
  String tmpStr = _menuString;
  if (tmpStr.length() > maxStringLength){
    tmpStr = _menuString.substring(0,maxStringLength);
    tmpStr.concat(":");
  }
 
  return tmpStr;
}

//Increment value whilst staying in bounds. 
short Menu::incrementVal(short incrementBy){

    lastValue=value;
    value += incrementBy;

    if (value < _minVal){
      if (_wrap){
        value = _maxVal;        
        incrementBy=-1;
      } else {
        value = _minVal;
      }
    }

    if (value > _maxVal){
      if (_wrap){
        value = _minVal;
        incrementBy=1;
      } else {
       value = _maxVal;
      }
    }

    return abs(incrementBy);
}

byte Menu::menuStringLength(){
  return menuString().length()+1;
}



void Menu::setValue(short VAL){
  value=VAL;
}

void Menu::setMax(short VAL){
  _maxVal=VAL;
}

void Menu::setMin(short VAL){
  _minVal=VAL;
}


void Menu::setMenuString(String menuString){
  _menuString=menuString;
}