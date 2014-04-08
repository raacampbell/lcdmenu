#ifndef Menu_h
#define Menu_h

#include "Arduino.h"







class Menu
{
  public:
    bool isVariable;
    byte value; //Value displayed on screen. If this is a link to another menu, value is the number of entries in that menu
    String menuString; //The menu string itself
    void (*buttonFunction) (); 

    Menu();
    void setNumericMenu(byte val,byte minVal,byte maxVal,bool wrap,String menuStr);
    void setActionMenu(String menuStr, void (*funct)() ); //Can link to any function that returns void
    void incrementVal(byte incrementBy);
    byte menuStringLength();

  private:
  	byte _minVal; //minimum allowable value 
    byte _maxVal; //maximum allowable value
    bool _wrap; //if 1 we wrap from min to max value

   
};


#endif