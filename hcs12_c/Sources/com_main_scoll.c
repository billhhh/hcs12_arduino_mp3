/**
 * author: Bill Wang
 * date: 12/8/2016
**/


#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "mp3_common.h"
#include <string.h>

#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

void SerTx1(unsigned char);
void MSDelay(unsigned int);
void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void DisplayStr(char*);
void CleanScr();
void LShiftStr(char*);

unsigned char column,row;
unsigned char calcol = 16;

void main(void) {
 
    int i;
    
    //init com
    SCI1BDH=0x00;    //48MHz/2=24Mhz, 24Mhz/16= 1.5MHz, 1.5MHz/9600=156
    SCI1BDL=156;     //Notice the value for Baud Rate
    SCI1CR1=0x00;
    SCI1CR2=0x0C;
    
    //init keypad
    DDRB = 0xFF;                           //MAKE PORTB OUTPUT
   DDRJ |=0x02; 
   PTJ &=~0x02;                            //ACTIVATE LED ARRAY ON PORT B
   DDRP |=0x0F;                           //
   PTP |=0x0F;                            //TURN OFF 7SEG LED
   DDRA = 0x0F;                           //MAKE ROWS INPUT AND COLUMNS OUTPUT
   
   //init lcd
   DDRK = 0xFF;   
  COMWRT4(0x33);   //reset sequence provided by data sheet
  MSDelay(1);
  COMWRT4(0x32);   //reset sequence provided by data sheet
  MSDelay(1);
  COMWRT4(0x28);   //Function set to four bit data length
                                   //2 line, 5 x 7 dot format
  MSDelay(1);
  COMWRT4(0x06);  //entry mode set, increment, no shift
  MSDelay(1);
  COMWRT4(0x0E);  //Display set, disp on, cursor on, blink off
  MSDelay(1);
  COMWRT4(0x01);  //Clear display
  MSDelay(1);
  COMWRT4(0x80);  //set start posistion, home position
  MSDelay(1);
   
 
   while(1){
   
      i = 0;                              //OPEN WHILE(1)
      do{                                 //OPEN do1
         PORTA = PORTA | 0x0F;            //COLUMNS SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         
         if(i>=50) {
          
           //scoll the name
           DisplayStr(scollName[calcol]);
           LShiftStr(scollName[calcol]);
           
           i = 0;
         }
         
         i++;
         
         MSDelay(10);
            
      }while(row == 0x00);                //WAIT UNTIL KEY PRESSED //CLOSE do1



      do{                                 //OPEN do2
         do{                              //OPEN do3
            MSDelay(1);                   //WAIT
            row = PORTA & 0xF0;           //READ ROWS
         }while(row == 0x00);             //CHECK FOR KEY PRESS //CLOSE do3
         
         MSDelay(15);                     //WAIT FOR DEBOUNCE
         row = PORTA & 0xF0;
      }while(row == 0x00);                //FALSE KEY PRESS //CLOSE do2


      while(1){                           //OPEN while(1)
      
         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x01;                   //COLUMN 0 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 0
            column = 0;
            break;                        //BREAK OUT OF while(1)
         }
         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x02;                   //COLUMN 1 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 1
            column = 1;
            break;                        //BREAK OUT OF while(1)
         }

         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x04;                   //COLUMN 2 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 2
            column = 2;
            break;                        //BREAK OUT OF while(1)
         }
         PORTA &= 0xF0;                   //CLEAR COLUMN
         PORTA |= 0x08;                   //COLUMN 3 SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
         if(row != 0x00){                 //KEY IS IN COLUMN 3
            column = 3;
            break;                        //BREAK OUT OF while(1)
         }
         row = 0;                         //KEY NOT FOUND
      break;                              //step out of while(1) loop to not get stuck
      }                                   //end while(1)
      
      //result
      if(row == 0x10){
         PORTB=keypad[0][column];         //OUTPUT TO PORTB LED
         SerTx1(keypad[0][column]);
         
         calcol = column;
      }
      else if(row == 0x20){
         PORTB=keypad[1][column];
         SerTx1(keypad[1][column]);

         calcol = column+4;
      }
      else if(row == 0x40){
         PORTB=keypad[2][column];
         SerTx1(keypad[2][column]);
 
         calcol = column+8;
      }
      else if(row == 0x80){
         PORTB=keypad[3][column];
         SerTx1(keypad[3][column]);
 
         calcol = column+12;
      }

      do{
         MSDelay(15);
         PORTA = PORTA | 0x0F;            //COLUMNS SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
      }while(row != 0x00);                //MAKE SURE BUTTON IS NOT STILL HELD
   }                                      //CLOSE WHILE(1)
  

}

void LShiftStr(char* str) {
  
  int i;
  char temp = str[0];  
  for(i=0;i<strlen(str)-1;++i)
    str[i] = str[i+1];    
  str[i] = temp;
}

void DisplayStr(char* str) {
  
  int i;
  CleanScr();
  
  for(i=0; str[i]!='\0' ;++i) {
    DATWRT4(str[i]);
    MSDelay(1);  
  }
}

void CleanScr() {
  COMWRT4(0x01);  //Clear display
  MSDelay(1);
  COMWRT4(0x80);  //set start posistion, home position
  MSDelay(1);  
}


void COMWRT4(unsigned char command)
  {
        unsigned char x;
        
        x = (command & 0xF0) >> 2;         //shift high nibble to center of byte for Pk5-Pk2
      LCD_DATA =LCD_DATA & ~0x3C;          //clear bits Pk5-Pk2
        LCD_DATA = LCD_DATA | x;          //sends high nibble to PORTK
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~RS;         //set RS to command (RS=0)
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;          //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;         //Drop enable to capture command
        MSDelay(15);                       //wait
        x = (command & 0x0F)<< 2;          // shift low nibble to center of byte for Pk5-Pk2
        LCD_DATA =LCD_DATA & ~0x3C;         //clear bits Pk5-Pk2
        LCD_DATA =LCD_DATA | x;             //send low nibble to PORTK
        LCD_CTRL = LCD_CTRL | EN;          //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;         //drop enable to capture command
        MSDelay(15);
  }

void DATWRT4(unsigned char data)
  {
  unsigned char x;
       
        
        
        x = (data & 0xF0) >> 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | RS;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(5);
       
        x = (data & 0x0F)<< 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(15);
  }

 void SerTx1(unsigned char c)
  {
    while(!(SCI1SR1 & 0x80));
    SCI1DRL=c;
  }
  
void MSDelay(unsigned int itime){
  unsigned int i; unsigned int j;
     for(i=0;i<itime;i++)
        for(j=0;j<4000;j++);
}