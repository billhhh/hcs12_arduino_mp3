;****************************************************************************************
;*TITLE:  serialcom_keypad.ASM                                                                    *
;*AUTHOR: Bill Wang                                                               *
;*DATE:   12-05-16                                                                      *
;*        Code modified for Dragon 12+ board derived from example 12.4 of                *
;*        HCS12 Microconntroller and Embedded Systems                                   *
;*        by: Mazidi and Causy                                                          *
;*Notes       : 1) Columns are to be treated as inputs Rows are outputs.                * 
;*                 Rows are set HIGH to start and brougnt low, if a column on that      *
;*                 row is found to be low, then that button value (0-15) is returned.   *
;*                                                                                      *
;*                   C0  C1  C2  C3                                                     *
;*                   PA0 PA1 PA2 PA3                                                    *
;*                    |   |   |   |                                                     *
;*                -----------------                                                     *
;*                |   |   |   |   |                                                     *
;*                | 0 | 1 | 2 | 3 |                                                     *
;*                ------------------- PA4, R0                                           *
;*                |   |   |   |   |                                                     *
;*                | 4 | 5 | 6 | 7 |                                                     *
;*                ------------------- PA5, R1                                           *
;*                |   |   |   |   |                                                     *
;*                | 8 | 9 | 10| 11|                                                     *
;*                ------------------- PA6, R2                                           *
;*                |   |   |   |   |                                                     *
;*                | 12| 13| 14| 15|                                                     *
;*                ------------------- PA7, R3                                           *
;****************************************************************************************


;Keypad program for Dragon12 Plus Trainer Board. 
;Press any key and the ASCII code for the key is shown on PORTB LEDs 
;with HCS12 Serial Monitor Program installed. This code is for CodeWarrior IDE
;In Dragon12+ RAM address is from $1000-3FFF 
;We use RAM addresses starting at $1000 for scratch pad (variables) and $3FFF for Stack 
;Make sure you are in HCS12 Serial Monitor Mode before downloading 
;and also make sure SW7=LOAD (SW7 is 2-bit red DIP Switch on bottom right side of the board and must be 00, or LOAD) 
;Press F7 (to Make), then F5(Debug) to downLOAD,and F5 once more to start the program execution
   
        ABSENTRY Entry        ; for absolute assembly: mark this as application entry point
    
; Include derivative-specific definitions 
		INCLUDE 'mc9s12dp256.inc'     ;CPU used by Dragon12+ board
 
 
;----------------------USE $1000-$2FFF for Scratch Pad 
D15mH EQU 17	
D15mL EQU 250	
COLS  EQU PORTA
ROWS  EQU PORTA
LED   EQU PORTB
ROW0  EQU %00010000
ROW1  EQU %00100000
ROW2  EQU %01000000
ROW3  EQU %10000000
COLM  EQU %00001111
ROWM  EQU %11110000
    ORG $1000           ;Keypad Look-up Table
KCODE0  DC.B '0123' 
KCODE1  DC.B '4567' 
KCODE2  DC.B '89AB' 
KCODE3  DC.B 'CDEF'

   ORG $1110           ;Delay VARIABLES 
DR15mH  DS.B 1	
DR15mL  DS.B 1
PDELAY  DS.B 1
          

;code section
   ORG   $4000     ;Flash ROM address for Dragon12+
Entry:
   LDS     #$4000    ;Stack
   LDAA	#$0		
   STAA  SCI1BDH    	
	 LDAA	#156		   ;For 9600 on Serial COM1
   STAA  SCI1BDL    ;48MHz/2=24MHz, 24MHz/16=1.5MHz and 1.5MHz/9600=156 	
   LDAA  #0	   	
   STAA	SCI1CR1	
   LDAA  #$0C	   	
   STAA	SCI1CR2	
   LDAA  #$FF         ;
   STAA  DDRB         ;MAKE PORTB OUTPUT
   lDAA  #$02         ;
   STAA  DDRJ         ;ENABLE LED ARRAY ON PORTB OUTPUT
   LDAA  #$00         ;
   STAA  PTJ          ;
   LDAA  #$F0         ;MAKE PORT A HIGH NIBLE OUTPUT
   STAA  DDRA         ;LOW NIBLE INPUT
   LDAA  #$0F         ;
   STAA  DDRP         ;
   STAA  PTP          ;TURN OFF 7SEG LED   	
   LDAA  #$00         ;
   STAA  PORTB        ;INITIALIZE PORT B
K1:                   ;TEST TO MAKE SURE NO BUTTON PRESSED
   LDAA  #ROWM        ;
   STAA  ROWS         ;SET ROWS HIGH
   LDAA  COLS         ;CAPTURE PORT A
   ANDA  #COLM        ;MASK OUT ROWS
   CMPA  #$00         ;
   BNE   K1           ;IF COLUMS IS ZERO NO BUTTON PRESSED
                      ;DO NOT MOVE ON UNTILL NO BUTTON IS PRESSED
K2:                   ;
   JSR   DELAY        ;CALL 15mS DELAY
   LDAA  COLS         ;
   ANDA  #COLM        ;
   CMPA  #$00         ;IF COLS !=0 THEN A BUTTON IS PRESSED
   BNE   OVER         ;
   BRA   K2           ;IF NO BUTTON PRESSED KEEP CHECKING
OVER:                 ;
   JSR   DELAY        ;DEBOUCE DELAY
   LDAA  COLS         ;READ PORT A
   ANDA  #COLM        ;MASK OUT ROWS
   CMPA  #$00         ;CHECK FOR PRESS AFTER DEBOUNCE
   BNE   OVER1        ;IF PRESSED GO FIND WHAT ROW
   BRA   K2           ;IF NO PRESS AFTER DEBOUNCE GO BACK
OVER1:                ;
   LDAA  #ROW0        ;MAKE HIGH ROW0 THE REST GROUNDED
   STAA  ROWS         ;
   MOVB  #$08,PDELAY  ;SET COUNT TO PROVIDE SHORT DELAY FOR STABILITY
P1:                   ;AFTER CHANGING THE PORT A OUTPUT
   DEC   PDELAY       ;DECREMENT COUNT
   BNE   P1           ;IF COUNT NOT ZERO KEEP DECREMENTING
   LDAA  COLS         ;READ PORTA
   ANDA  #COLM        ;MASK OUT ROWS
   CMPA  #$00         ;IS INPUT ZERO?
   BNE   R0           ;IF COLUMS NOT ZERO THEN BUTTON IS IN ROW 0
   LDAA  #ROW1        ;IF ZERO THEN BUTTON NOT IN ROW0
   STAA  ROWS         ;TURN ON ROW 1 TURN OFF ALL OTHERS
   MOVB  #$08,PDELAY  ;SHORT DELAY TO STABALIZE
P2:                   ;AFTER CHANGING THE PORT A OUTPUT
   DEC   PDELAY       ;DECREMENT COUNT
   BNE   P2           ;IF COUNT NOT ZERO KEEP DECREMENTING
   LDAA  COLS         ;READ PORT A
   ANDA  #COLM        ;MASK OUT ROWS
   CMPA  #$00         ;CHECK FOR KEY PRESS
   BNE   R1           ;IF PRESSED KEY IS IN ROW1
   LDAA  #ROW2        ;IF ZERO BUTTON NOT IN ROW1
   STAA  ROWS         ;TURN ON ROW2 ALL OTHERS OFF
   MOVB  #$08,PDELAY  ;SHORT DELAY TO STABALIZE PORTA
P3:                   ;
   DEC   PDELAY       ;DECREMENT COUNT
   BNE   P3           ;DELAY LOOP 
   LDAA  COLS         ;READ PORTA
   ANDA  #COLM        ;MASK OUT ROWS
   CMPA  #$00         ;CHECK FOR PRESS
   BNE   R2           ;IF FOUND KEY IS IN ROW2
   LDAA  #ROW3        ;IF ZERO MOVE TO ROW3
   STAA  ROWS         ;TURN ON ROW3 ALL OTHERS OFF
   MOVB  #$08,PDELAY  ;SHORT DELAY TO STABALIZE OUTPUT
P4:                   ;
   DEC   PDELAY       ;DECREMENT DELAY
   BNE   P4           ;DELAY LOOP
   LDAA  COLS         ;READ PORT A
   ANDA  #COLM        ;MASK OUT ROWS
   CMPA  #$00         ;CHECK FOR PRESS
   BNE   R3           ;IF FOUND KEY IN ROW3
   BRA   K2           ;IF ROW NOT FOUND GO BACK TO START     	
R0:                   ;
   LDX   #KCODE0      ;LOAD PONTER TO ROW0 ARRAY	
   BRA   FIND         ;GO FIND COLUMN	 	
R1:                   ;
   LDX   #KCODE1      ;LOAD POINTER TO ROW1 ARRAY	
   BRA   FIND         ;GO FIND COUMN		
R2:                   ;
   LDX   #KCODE2      ;LOAD PINTER TO ROW2
   BRA   FIND         ;GO FIND COLUMN
R3:                   ;
   LDX   #KCODE3      ;LOAD POINTER TO ROW3
   BRA   FIND         ;GO FIND COLUMN
FIND:                 ;
   ANDA  #COLM        ;MASK OUT ROWS
   COMA               ;INVERT COLUMN
SHIFT:                ;
   LSRA               ;LOGICAL SHIFT RIGHT PORTA
   BCC   MATCH        ;IF CARY CLEAR COLUM IS FOUND
   INX                ;IF CARY NOT CLEAR INCREMENT POINTER TO ROW ARRAY
   BRA   SHIFT        ;SHIFT RIGHT UNTIL CARY IS CLEAR.
MATCH:                ;
   LDAA  0,X  
   JSR	TRANS1                 ;LOAD ASCII FROM ROW ARARAY
   LBRA  K1           ;BACK TO START		
   
   
   
   
   
;---------------------------------DELAY   
DELAY:                ;DELAY SUBROUTINE
   LDAA  #D15mH       ;LOAD FIRST LAYER DELAY COUNT
   STAA  DR15mH       ;
D2:                   ;
   LDAA  #D15mL       ;LOAD SECOND LAYER DELAY COUNT
   STAA  DR15mL       ;
D1:                   ;
   DEC   DR15mL       ;DECREMENT SECOND LAYER COUNT
   NOP                ;NOP DELAY
   NOP                ;
   BNE   D1           ;BACK TO START OF SECOND LAYER IF NOT ZERO
   DEC   DR15mH       ;IF SECOND LAYER ZERO DECREMENT FIRST LAYER
   BNE   D2           ;IF FIRST LAYER NOT ZERO GO BACK RELOAD FRIST LAYER
   RTS                ;RETURN TO FROM SUBROUTINE WEHN LAYER ONE IS ZERO   
        
        
        
TRANS1 
HERE: BRCLR	SCI1SR1, mSCI0SR1_TDRE, HERE	
  	  STAA	SCI1DRL	
	    RTS	        
        
;-------------------------        
       
            
;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************
            ;ORG   $FFFE
            DC.W  Entry     ;Reset Vector. CPU wakes here and it is sent to start of the code at $4000
