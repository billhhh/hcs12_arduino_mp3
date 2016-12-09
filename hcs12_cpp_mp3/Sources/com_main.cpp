/**
 * author: Bill Wang
 * date: 12/8/2016
**/

   

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */


void SerTx1(unsigned char);
void mSDelay(unsigned int);

const unsigned char keypad[4][4] =
{
'0','1','2','3',
'4','5','6','7',
'8','9','A','B',
'C','D','E','F'
};

unsigned char column,row;

void main(void) {
 

    SCI1BDH=0x00;    //48MHz/2=24Mhz, 24Mhz/16= 1.5MHz, 1.5MHz/9600=156
    SCI1BDL=156;     //Notice the value for Baud Rate
    SCI1CR1=0x00;
    SCI1CR2=0x0C;
    
    DDRB = 0xFF;                           //MAKE PORTB OUTPUT
   DDRJ |=0x02; 
   PTJ &=~0x02;                            //ACTIVATE LED ARRAY ON PORT B
   DDRP |=0x0F;                           //
   PTP |=0x0F;                            //TURN OFF 7SEG LED
   DDRA = 0x0F;                           //MAKE ROWS INPUT AND COLUMNS OUTPUT
   
 
   while(1){                              //OPEN WHILE(1)
      do{                                 //OPEN do1
         PORTA = PORTA | 0x0F;            //COLUMNS SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
      }while(row == 0x00);                //WAIT UNTIL KEY PRESSED //CLOSE do1



      do{                                 //OPEN do2
         do{                              //OPEN do3
            mSDelay(1);                   //WAIT
            row = PORTA & 0xF0;           //READ ROWS
         }while(row == 0x00);             //CHECK FOR KEY PRESS //CLOSE do3
         
         mSDelay(15);                     //WAIT FOR DEBOUNCE
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
 
      }
      else if(row == 0x20){
         PORTB=keypad[1][column];
         SerTx1(keypad[1][column]);
 
      }
      else if(row == 0x40){
         PORTB=keypad[2][column];
         SerTx1(keypad[2][column]);
 
      }
      else if(row == 0x80){
         PORTB=keypad[3][column];
         SerTx1(keypad[3][column]);
 
      }

      do{
         mSDelay(15);
         PORTA = PORTA | 0x0F;            //COLUMNS SET HIGH
         row = PORTA & 0xF0;              //READ ROWS
      }while(row != 0x00);                //MAKE SURE BUTTON IS NOT STILL HELD
   }                                      //CLOSE WHILE(1)
  

}

 void SerTx1(unsigned char c)
  {
    while(!(SCI1SR1 & 0x80));
    SCI1DRL=c;
  }
  
void mSDelay(unsigned int itime){
  unsigned int i; unsigned int j;
     for(i=0;i<itime;i++)
        for(j=0;j<4000;j++);
}