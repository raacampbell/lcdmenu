/*                                                                                
 Set up a menu system using a class 
*/


// Include the LCD library
#include "LiquidCrystal.h"
#include "settings.h"
#include "Menu.h"



//Define the settings menu
const int settingsMenuLength=6;
Menu settingsMenu[settingsMenuLength] ;
Menu* thisMenuPtr = settingsMenu;
byte nMenuRows=settingsMenuLength; //Number of entries in settingsMenu

//Define another menu (mainMenu) to link to as an example
const int mainMenuLength=3;
Menu mainMenu[mainMenuLength];



void setup() {
  Serial.begin(9600);          //  setup serial
  pinMode(buttonPin,INPUT);

  //Define the settings menu
  settingsMenu[0].setNumericMenu( 4, 1,  10, 1, "Taste 1 Angle:");
  settingsMenu[1].setNumericMenu(45, 1, 180, 1, "dAngle:");
  settingsMenu[2].setNumericMenu(80, 2, 178, 0, "Extended pos:");
  settingsMenu[3].setNumericMenu(70, 2, 178, 0, "Retracted pos:");
  settingsMenu[4].setActionMenu("To main menu",toMainMenu);
  settingsMenu[5].setActionMenu("Wipe and re-draw",wiper);

  //Define the mainMenu
  mainMenu[0].setNumericMenu(4, 1,  10, 1, "Something:");
  mainMenu[1].setNumericMenu(4, 1,  10, 1, "Something ELSE:");
  mainMenu[2].setActionMenu("BACK",toSettingsMenu);


  // set up the LCD's number of columns and rows: 
  lcd.begin(lcdCols, lcdRows);
  lcd.createChar(0, upArrow);
  lcd.createChar(1, downArrow);  

  //Bring up the menu
  refreshScreen();  
  /*
  Serial.println("Start debug");
  Serial.println(sizeof(thisMenuPtr));
  Serial.println(sizeof(thisMenuPtr[0]));
  Serial.println(sizeof(thisMenuPtr)/sizeof(thisMenuPtr[0]));
  Serial.println("End debug");
*/
}



void loop() {
  
  //The following variables are used to tell for how long the 
  //user has been pushing at the stick. This allows us to 
  //increment numbers by 10x or 100x, as desired. 
  static unsigned long numIncrements=0; //The number of continuous increments 
  static bool lastIncremented=0; //Was the variable incremented in the last pass?

  //Following variables are "wait-without-delay" counters for listening to the 
  //thumbstick. By default the thumbstick code will be executed every varInterval ms. 
  //However, this rate is modifed by the stick position. We want to check more often 
  //when the stick is untouched (improved responsiveness) and less often whilst we're 
  //scrolling (to maintain reasonable scroll speed). 
  static long varInterval=25; 
  static unsigned long currentVarMillis;
  static long previousVarMillis=0;


  //Read stick every varInterval ms (determined by last-read stick position)
  //and update screen as needed. 
  currentVarMillis=millis();
  if(currentVarMillis - previousVarMillis > varInterval) {  
    previousVarMillis=currentVarMillis; //save update time
        

    //Read the X and Y directions of the stick
    short y=analogRead(A1);
    short x=analogRead(A0);

    short delta_y=y-centre;  
    short delta_x=x-centre;  


    //Determines the speed with which the screen updates during 
    //user interaction 
    if (abs(delta_y)>deadZone || abs(delta_x)>deadZone){
       varInterval=centre-abs(x-centre)*0.6; 
    } else {
       varInterval=25;
    }



    //Move the little arrow indicating the line we're on. Scroll
    //screen if appropriate. 
    if (abs(delta_y)>deadZone){
     if (y>centre){
       arrowPos--;
      } else if (y<centre){
       arrowPos++;
      }
     updateCursor();
    }//if (abs(delta_y)...


    //Allow updates of the values on the current row
    byte menuRow=arrowPos+currentTopRow; //The menu row (allows for scrolling)

    if (abs(delta_x)>deadZone){
      byte incrementBy; //By how much to increment   

      if (numIncrements>=4 && (thisMenuPtr[menuRow].value % 10)==0)
      {
       incrementBy=10;
      } else {
       incrementBy=1;
      }

      if (x>centre)
      {
       thisMenuPtr[menuRow].incrementVal(incrementBy);
      } else if (x<centre){
       thisMenuPtr[menuRow].incrementVal(-incrementBy);
      }
      
     
     

      updateRowValue(menuRow);
      
      
      if (lastIncremented){
	      numIncrements++;
      }
      lastIncremented=1;
    } else {
      lastIncremented=0;
      numIncrements=0;
    }//if (abs(delta_x)...


    //Finally, check if button is pressed and, if so, run the appropriate function if this is possible
    if (digitalRead(buttonPin)==HIGH){
      if (!thisMenuPtr[menuRow].isVariable){
        thisMenuPtr[menuRow].buttonFunction();
      }
    } 

  } //if(currentVarMillis ...
  
} //function loop





//re-draw the whole screen
 
 void refreshScreen(){

 byte screenRow=0;
 for (byte menuRow=currentTopRow; menuRow<currentTopRow+lcdRows; menuRow++){
   if (screenRow>=nMenuRows){
    lcd.setCursor(0,screenRow++);
    for (int ii=0; ii<lcdCols; ii++){  lcd.print(" "); } //wipe whole line
   } else {
    lcd.setCursor(1,screenRow++);
    lcd.print(thisMenuPtr[menuRow].menuString);
    updateRowValue(menuRow);
   } // if (screenRow>nMenuRows)
   
 }


 updateCursor(); //The highlighted line
 
 //Add up or down arrows depending on where we are in the
 //menu system
 lcd.setCursor(lcdCols-1,0); lcd.print(" ");
 lcd.setCursor(lcdCols-1,lcdRows-1); lcd.print(" ");

 if (currentTopRow>0){
   lcd.setCursor(lcdCols-1,0);
   lcd.write(byte(0));
 }
 if ((currentTopRow+lcdRows)<nMenuRows){
   lcd.setCursor(lcdCols-1,lcdRows-1);
   lcd.write(byte(1));
 }
 
 
} //void refreshScreen


//update the little arrow (cursor) as the user pushes the thumbstick
void updateCursor(){
 byte lastRow;

 if (nMenuRows>lcdRows){
   lastRow=lcdRows;
 } else {
   lastRow=nMenuRows;
 }

 //clear arrow position row
 for (byte ii=0; ii<lcdRows; ii++){
  lcd.setCursor(0,ii);
  lcd.print(" ");
 }

 //If arrow is within bounds we print it
 if (arrowPos>=0 && arrowPos<lastRow){
   printArrow(arrowPos);
   return;
 } 

 //If arrow has moved beyond the lowest menu row but this isn't the lowest
 //lcd row, then don't allow arrow beyond this point.  
 if (arrowPos>=lastRow && lastRow<lcdRows){
   arrowPos=lastRow-1;
   printArrow(arrowPos);
   return;
 } 


 //If arrow has gone beyond the top row, we keep it at top row
 if (arrowPos < 0){
   arrowPos=0;
   printArrow(arrowPos);
   if (currentTopRow>0){ //If scroll upwards if required
      currentTopRow--;
      refreshScreen();
   }

   return;
 }


 //If arrow has gone beyond the bottom lcd row, we keep it at bottom row
 if (arrowPos>=lcdRows){
   arrowPos=lcdRows-1;
   printArrow(arrowPos);   
   if (nMenuRows-lcdRows-currentTopRow > 0){ //If there are more rows to go we scroll down
     currentTopRow++;
     refreshScreen();   
   }

   return;
 }

   
} //void updateCursor


void updateRowValue(byte menuRow){
   
   //Wipe the region of the row where the value will be placed
   //byte cursorPos=thisMenuPtr[menuRow].menuStringLength+1;
   byte cursorPos=thisMenuPtr[menuRow].menuStringLength();
   byte screenRow=menuRow-currentTopRow;


   for (byte ii=cursorPos; ii<=lcdCols-2; ii++){
    lcd.setCursor(ii,screenRow); 
    lcd.print(" ");  
   }
   
   if (thisMenuPtr[menuRow].isVariable){
     lcd.setCursor(cursorPos,screenRow); 
     lcd.print(thisMenuPtr[menuRow].value);  
   }

} //void updateRow


void printArrow(byte screenRow){
   lcd.setCursor(0,screenRow);
   lcd.print(">");
}


//TEST STUFF BELOW
void wiper(){
  lcd.clear();
  delay(3000);
  refreshScreen();
}

void toMainMenu(){
   nMenuRows=mainMenuLength;
   thisMenuPtr=mainMenu; //Switch to new menu
   currentTopRow=0;
   arrowPos=0;
   refreshScreen();
}

void toSettingsMenu(){
   nMenuRows=settingsMenuLength;
   thisMenuPtr=settingsMenu; //Switch to new menu
   currentTopRow=0;
   arrowPos=0;
   refreshScreen();
}
