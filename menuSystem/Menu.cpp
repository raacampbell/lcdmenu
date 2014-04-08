
/*
  Menu.cpp - Library for building LCD menu
*/

#include "Arduino.h"
#include "Menu.h"



Menu::Menu(){

}



void Menu::setNumericMenu(byte val,byte minVal,byte maxVal,bool wrap,String menuStr)
{
	isVariable=1;
	value=val;
	menuString=menuStr;
	_maxVal=maxVal;
	_minVal=minVal;
	_wrap=wrap;
}

void Menu::setActionMenu(String menuStr, void (*funct)() )
{
	isVariable=0;
	menuString=menuStr;
	buttonFunction=funct;
}


//Increment value whilst staying in bounds. 
void Menu::incrementVal(byte incrementBy){
	value += incrementBy;

    if (value < _minVal){
      if (_wrap){
        value = _maxVal;
      } else {
        value = _minVal;
      }
    }

    if (value > _maxVal){
      if (_wrap){
        value = _minVal;
      } else {
       value = _maxVal;
      }
    }
}

byte Menu::menuStringLength(){
	return menuString.length()+1;
}
