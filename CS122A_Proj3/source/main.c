/*	Author: Zachary Hill
 *	Assignment: Project 3
 *	Exercise Description: Grocery Self-Checkout Simulation
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

typedef struct task {
  int state;                  // Task's current state
  unsigned long period;       // Task period
  unsigned long elapsedTime;  // Time elapsed since last task tick
  int (*TickFct)(int);        // Task tick function
} task;

task tasks[4];
const unsigned short tasksNum = 4;

void TimerOn() {
  TCCR1B = 0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;
  _avr_timer_cntcurr = _avr_timer_M;
  SREG |= 0x80;
}

void TimerOff() {
  TCCR1B = 0x00;
}

void TimerISR() {
  TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void data (unsigned char row, unsigned char col, unsigned char item) {
   LCD_Cursor((col+1) + (row-1) * 15);
   LCD_WriteData(item);
}

//start of synchSms

enum CH_States {CH_SMStart, CH_Begin, CH_On, CH_Blink, CH_Reset, CH_Select} CH_State;

unsigned char rndmLED = 0;
unsigned char activate = 0;

void TickFct_Checkout() {   //Tick Function that displays the checkout lights as LEDs
   
   switch ( CH_State ) { //Transitions
      case CH_SMStart:
         CH_State = CH_Begin; //Initial state
         break;
      case CH_Begin:
         CH_State = CH_On;
         break;
      case CH_On:
         if ((~PINA & 0x7F) == 0x02) {
            CH_State = CH_Reset;
         }
         else if ((~PINA & 0x7F) == 0x01) {
            CH_State = CH_Select;
         }
         else if ((~PINA & 0x7F) == 0x00) {
            CH_State = CH_Blink;
         }
         break;
      case CH_Blink:
         if ((~PINA & 0x7F) == 0x02) {
            CH_State = CH_Reset;
         }
         else if ((~PINA & 0x7F) == 0x01) {
            CH_State = CH_Select;
         }
         else if ((~PINA & 0x7F) == 0x00) {
            CH_State = CH_On;
         }
         break;
      case CH_Reset:
         if ((~PINA & 0x7F) == 0x02) {
            CH_State = CH_Reset;
         }
         else if ((~PINA & 0x7F) == 0x00) {
            CH_State = CH_On;
         }
         break;
      case CH_Select:
         if ((~PINA & 0x7F) == 0x01) {
            CH_State = CH_Select;
         }
         else if ((~PINA & 0x7F) == 0x00) {
            CH_State = CH_On;
         }
         break;
      default:
         CH_State = CH_SMStart;
         break;
   }

   switch (CH_State ) { //State actions
      case CH_SMStart:
         break;
      case CH_Begin:
	 rndmLED = (rand() % 31) + 1;
         break;
      case CH_On:
         PORTB = rndmLED;
         break;
      case CH_Blink:
         PORTB = 0x00;
         break;
      case CH_Reset:
         rndmLED = (rand() % 31) + 1;
         activate = 0;
         LCD_ClearScreen();
         break;
      case CH_Select:
         if ((rndmLED <= 31 && rndmLED >= 16) && (((~PINB & 0x1F) == 0x1F) || ((~PINB & 0x1F) == 0x1E) || ((~PINB & 0x1F) == 0x1D) || ((~PINB & 0x1F) == 0x1C) || ((~PINB & 0x1F) == 0x1B) || ((~PINB & 0x1F) == 0x1A) || ((~PINB & 0x1F) == 0x19) || ((~PINB & 0x1F) == 0x18) || ((~PINB & 0x1F) == 0x17) || ((~PINB & 0x1F) == 0x16) || ((~PINB & 0x1F) == 0x15) || ((~PINB & 0x1F) == 0x14) || ((~PINB & 0x1F) == 0x13) || ((~PINB & 0x1F) == 0x12) || ((~PINB & 0x11) == 0x15) || ((~PINB & 0x1F) == 0x10))) {
            if((~PINA & 0x80) == 0x01) {
        	if ((~PINA & 0x7F) == 0x01) {
               		activate = 2;
		}
            }
		else if((~PINA & 0x80) == 0x00) {
            		if ((~PINA & 0x7F) == 0x01) {
               			activate = 1;
	    		}
        	}
         }
         else if ((rndmLED <= 15 && rndmLED >= 8) && (((~PINB & 0x1F) == 0x0F) || ((~PINB & 0x1F) == 0x0E) || ((~PINB & 0x1F) == 0x0D) || ((~PINB & 0x1F) == 0x0C) || ((~PINB & 0x1F) == 0x0B) || ((~PINB & 0x1F) == 0x0A) || ((~PINB & 0x1F) == 0x09) || ((~PINB & 0x1F) == 0x08))) {
            if((~PINA & 0x80) == 0x01) {
        	if ((~PINA & 0x7F) == 0x01) {
               		activate = 2;
		}
            }
         }
         else if ((rndmLED <= 7 && rndmLED >= 4) && (((~PINB & 0x1F) == 0x07) || ((~PINB & 0x1F) == 0x06) || ((~PINB & 0x1F) == 0x05) || ((~PINB & 0x1F) == 0x04))) {
            if((~PINA & 0x80) == 0x01) {
        	if ((~PINA & 0x7F) == 0x01) {
               		activate = 2;
		}
            }
         }
         else if ((rndmLED == 3 || rndmLED == 2) && (((~PINB & 0x1F) == 0x03) || ((~PINB & 0x1F) == 0x02))) {
            if((~PINA & 0x80) == 0x01) {
        	if ((~PINA & 0x7F) == 0x01) {
               		activate = 2;
		}
            }
         }
         else if ((rndmLED == 1) && ((~PINB & 0x1F) == 0x01)) {
            if((~PINA & 0x80) == 0x01) {
        	if ((~PINA & 0x7F) == 0x01) {
               		activate = 2;
		}
            }
         }
         break;
      default:
         break;
   }
}

enum SC_States {SC_SMStart, SC_Wait, SC_Start, SC_Scan} SC_State;

//Prices in 2-d array: Drinks (Milk, Orange Juice, Soda), Produce (Apples, Bananas, Grapes), 
//Meat (Ribs, Shrimp, Burger Meat), Cans (Peas, Corn, Raviolis)
//Array contains prices based on sizes of each individual item; increasing in size

//const char* price[12][3] = {{"1.00", "1.50", "2.75"}, {"1.00", "1.25", "2.00"}, {"1.05", "2.80", "3.64"}, {"1.00", "1.50", "2.00"}, {"1.25", "1.80", "2.10"}, {"1.10", "1.55", "1.80"}, {"3.26", "4.38", "5.10"}, {"4.30", "5.22", "5.90"}, {"2.44", "3.56", "4.28"}, {"0.85", "1.25", "1.60"}, {"0.85", "1.25", "1.60"}, {"1.05", "1.65", "2.00"}};
//const unsigned char food_ID[12] = {"Milk", "OJ", "Soda", "Apples", "Bananas", "Grapes", "Ribs", "Shrimp", "B-Meat", "Peas", "Corn", "Raviolis"};
double price[12][3] = {{1.00, 1.50, 2.75}, {1.00, 1.25, 2.00}, {1.05, 2.80, 3.64}, {1.00, 1.50, 2.00}, {1.25, 1.80, 2.10}, {1.10, 1.55, 1.80}, {3.26, 4.38, 5.10}, {4.30, 5.22, 5.90}, {2.44, 3.56, 4.28}, {0.85, 1.25, 1.60}, {0.85, 1.25, 1.60}, {1.05, 1.65, 2.00}};

unsigned char i;
double amt = 0.00;
unsigned char position[2] = {4, 20};
unsigned char htag[2] = {15, 31};
unsigned char scan = 0, more1 = 0, pay = 0;
unsigned char rndmGrp, rndmItem;

void TickFct_Scanner() {   //Tick Function that operates the item moving toward scanner on LED screen
   
   switch ( SC_State ) { //Transitions
      case SC_SMStart:
         SC_State = SC_Wait; //Initial state
         break;
      case SC_Wait:
         if(activate == 1) {
            SC_State = SC_Start;
         }
         else {
            SC_State = SC_Wait;  
         }
         break;
      case SC_Start:
	 if((~PINA & 0x7F) == 0x02) {
		SC_State = SC_Wait;
		amt = 0;
		pay = 0;
	 }
         else {
         	if (scan == 1) {
            		SC_State = SC_Scan;
         	}	
         	else {
            		SC_State = SC_Start;
         	}
	 }
         break;
      case SC_Scan:
	 if((~PINA & 0x7F) == 0x02) {
		SC_State = SC_Wait;
		amt = 0;
		pay = 0;
	 }
         else {
         	if (more1 == 1) {
            		SC_State = SC_Start;
         	}
         	else {
            		SC_State = SC_Scan;
         	}
	 }
         break;
      default:
         SC_State = SC_SMStart;
         break;
   }

   switch (SC_State ) { //State actions
      case SC_SMStart:
         break;
      case SC_Wait:
         rndmGrp = 0;
         rndmItem = 0;
         scan = 0;
         more1 = 0;
         break;
      case SC_Start:
         rndmGrp = (rand() % 3);
         rndmItem = (rand() % 12);
         for(i=0; i<2; i++) {
	    data(1, position[i], '*'); //position of scanner
            if(htag[i] == 0) {
               htag[i] = 15;
               data(1,0, ' ');
            }
	    else if(htag[i] == 16) {
               htag[i] = 31;
               data(1,16, ' ');
            }
            else {
               htag[i]--;
            }
            data(1, htag[i], '#');     //moving ojects
            data(1, htag[i]+1, ' ');
            
            if((~PINA & 0x7F) == 0x04) {
               if(position[i] == htag[i]) {
                  scan = 1;
               }
            }
         }
         break;
      case SC_Scan:
         amt += price[rndmItem][rndmGrp];
         LCD_DisplayString(17, "Cont.?  Y  or  N");
         if((~PINA & 0x7F) == 0x40) {
            more1 = 1;
	    LCD_ClearScreen();
         }
         else if((~PINA & 0x7F) == 0x20) {
            more1 = 0;
            pay = 1;
	    LCD_ClearScreen();
	    activate = 0;
         }
         break;
      default:
         break;
   }
}


enum GP_States {GP_SMStart, GP_Wait, GP_Aisle, GP_Item, GP_Size, GP_More} GP_State;

//Arrays for names of items in each group and their possible sizes
//const unsigned char food[4][3] = {{"Milk", "OJ", "Soda"}, {"Apples", "Bananas", "Grapes"}, {"Ribs", "Shrimp", "Burger Meat"}, {"Peas", "Corn", "Raviolis"}};
//const unsigned char size[12][3] = {{"1)Bottle", "2)Quart", "3)Gallon"}, {"1)Bottle", "2)Quart", "3)Gallon"}, {"1)Liter", "2)12 cans", "3)6 b-pack"}, {"1)2 cnt", "2)4 cnt", "3)6 count"}, {"1)2 bnd", "2)4 bnd", "3)6 bundle"}, {"1)SM bg", "2)MDM bg", "3)LRG bag"}, {"1)SM pk", "2)MDM pk", "3)LRG package"}, {"1)SM pk", "2)MDM pk", "3)LRG package"}, {"1)SM pk", "2)MDM pk", "3)LRG package"}, {"1)SM cn", "2)MDM cn", "3)LRG can"}, {"1)SM cn", "2)MDM cn", "3)LRG can"}, {"1)SM cn", "2)MDM cn", "3)LRG can"}};

unsigned char group_choice = 0;
unsigned char item_choice = 0;
unsigned char sz_choice = 0;
unsigned char more2 = 0;

void TickFct_Stock() {   //Tick Function that operates the item moving toward scanner on LED screen
   
   switch ( GP_State ) { //Transitions
      case GP_SMStart:
         GP_State = GP_Wait; //Initial state
         break;
      case GP_Wait:
	if (activate == 2) {
         	GP_State = GP_Aisle;
	}
	else {
		GP_State = GP_Wait;
	}
         break;
      case GP_Aisle:
	if((~PINA & 0x7F) == 0x02) {
		GP_State = GP_Wait;
		amt = 0;
		pay = 0;
	}
	else {
         if ((~PINA & 0x7F) == 0x08 || (~PINA & 0x7F) == 0x10 || (~PINA & 0x7F) == 0x20 || (~PINA & 0x7F) == 0x40) {
               if ((~PINA & 0x7F) == 0x08) {
                  group_choice = 3;
               }
               else if ((~PINA & 0x7F) == 0x10) {
                  group_choice = 2;
               }
               else if ((~PINA & 0x7F) == 0x20) {
                  group_choice = 1;
               }
               else if ((~PINA & 0x7F) == 0x40) {
                  group_choice = 0;
               }
               GP_State = GP_Item;
         }
         else {
            GP_State = GP_Aisle; 
         }
	}
         break;
      case GP_Item:
	if((~PINA & 0x7F) == 0x02) {
		GP_State = GP_Wait;
		amt = 0;
		pay = 0;
	}
	else {
         if ((~PINA & 0x7F) == 0x10 || (~PINA & 0x7F) == 0x20 || (~PINA & 0x7F) == 0x40) {
            if ((~PINA & 0x7F) == 0x10) {
               item_choice = group_choice*3 + 2;
            }
            else if ((~PINA & 0x7F) == 0x20) {
               item_choice = group_choice*3 + 1;
            }
            else if ((~PINA & 0x7F) == 0x40) {
               item_choice = group_choice*3;
            }
            GP_State = GP_Size; 
         }
         else {
            GP_State = GP_Item; 
         }
         break;
	}
      case GP_Size:
	if((~PINA & 0x7F) == 0x02) {
		GP_State = GP_Wait;
		amt = 0;
		pay = 0;
	}
	else {
         if ((~PINA & 0x7F) == 0x10 || (~PINA & 0x7F) == 0x20 || (~PINA & 0x7F) == 0x40) {
            if ((~PINA & 0x7F) == 0x10) {
               sz_choice = 2;
            }
            else if ((~PINA & 0x7F) == 0x20) {
               sz_choice = 1;
            }
            else if ((~PINA & 0x7F) == 0x40) {
               sz_choice = 0;
            }
            GP_State = GP_More; 
         }
         else {
            GP_State = GP_Size; 
         }
	}
         break;
      case GP_More:
	if((~PINA & 0x7F) == 0x04) {
		GP_State = GP_Wait;
	}
	else {
         if (more2 == 1) {
            GP_State = GP_Aisle; 
         }
         else {
            GP_State = GP_More; 
         }
	}
         break;
      default:
         GP_State = GP_SMStart;
         break;
   }

   switch (GP_State ) { //State actions
      case GP_SMStart:
         break;
      case GP_Wait:
	 group_choice = 0;
         item_choice = 0;
         sz_choice = 0;
         more2 = 0;
         break;
      case GP_Aisle:
            LCD_DisplayString(1, "Drinks  Produce Meat   CanGoods ");
         break;
      case GP_Item:
         //LCD_DisplayString(1, food[group_choice][0]);
         //LCD_DisplayString(9, food[group_choice][1]);
         //LCD_DisplayString(19, food[group_choice][2]);
	 if(group_choice == 0) {
		LCD_DisplayString(1, "Milk    OJ        Soda          ");
	 }
	 else if(group_choice == 1) {
		LCD_DisplayString(1, "Apples  Bananas   Grapes        ");
	 }	
	 else if(group_choice == 2) {
		LCD_DisplayString(1, "Ribs    Shrimp    Burger Meat   ");
	 }
         else if(group_choice == 3) {
		LCD_DisplayString(1, "Peas    Corn      Raviolis      ");
	 }	
         break;
      case GP_Size:
         //LCD_DisplayString(1, size[item_choice][0]);
         //LCD_DisplayString(9, size[item_choice][1]);
         //LCD_DisplayString(19, size[item_choice][2]);
	 if(item_choice == 0) {
		LCD_DisplayString(1, "1)Bottle 2)Quart  3)Gallon      ");
	 }
	 else if(item_choice == 1) {
		LCD_DisplayString(1, "1)Bottle 2)Quart  3)Gallon      ");
	 }	
	 else if(item_choice == 2) {
		LCD_DisplayString(1, "1)Liter 2)12cans  3)6 b-pack    ");
	 }
         else if(item_choice == 3) {
		LCD_DisplayString(1, "1)2 cnt  2)4 cnt  3)6 count     ");
	 }
         else if(item_choice == 4) {
		LCD_DisplayString(1, "1)2 bnd  2)4 bnd  3)6 bundle    ");
	 }
	 else if(item_choice == 5) {
		LCD_DisplayString(1, "1)SM bg 2)MDM bg  3)LRG bag     ");
	 }	
	 else if(item_choice == 6) {
		LCD_DisplayString(1, "1)SM pk 2)MDM pk  3)LRG package ");
	 }
         else if(item_choice == 7) {
		LCD_DisplayString(1, "1)SM pk 2)MDM pk  3)LRG package ");
	 }
	 else if(item_choice == 8) {
		LCD_DisplayString(1, "1)SM pk 2)MDM pk  3)LRG package ");
	 }
	 else if(item_choice == 9) {
		LCD_DisplayString(1, "1)SM cn 2)MDM cn  3)LRG can     ");
	 }
         else if(item_choice == 10) {
		LCD_DisplayString(1, "1)SM cn 2)MDM cn  3)LRG can     ");
	 }
	 else if(item_choice == 11) {
		LCD_DisplayString(1, "1)SM cn 2)MDM cn  3)LRG can     ");
	 }
         break; 
      case GP_More:
         amt += price[item_choice][sz_choice];
         
         LCD_DisplayString(3, "Yes?     No?      ADD MORE    ");
         if ((~PINA & 0x7F) == 0x40) {
            more2 = 1;
         }
         else if ((~PINA & 0x7F) == 0x20) {
            pay = 1;
	    more2 = 0;
	    activate = 0;
         }
         break; 
      default:
         break;
   }
}

enum PAY_States {PAY_SMStart, PAY_Blank, PAY_Price} PAY_State;

double receipt = 0.00;

void TickFct_Receipt() {   //Tick Function that operates the item moving toward scanner on LED screen
   
   switch ( PAY_State ) { //Transitions
      case PAY_SMStart:
         PAY_State = PAY_Blank; //Initial state
         break;
      case PAY_Blank:
         if(pay) {
            PAY_State = PAY_Price;
         }
         else {
            PAY_State = PAY_Blank;
         }
         break;
      case PAY_Price:
         if ((~PINA & 0x7F) == 0x02) {
            PAY_State = PAY_Blank;
         }
         else {
            PAY_State = PAY_Price;
         }
         break;
      default:
         PAY_State = PAY_SMStart;
         break;
   }

   switch (PAY_State ) { //State actions
      case PAY_SMStart:
         break;
      case PAY_Blank:
         receipt = 0.00;
         break;
      case PAY_Price:
         receipt = amt;
         //LCD_WriteFloat(receipt);
	 LCD_DisplayString(17, "^^ Total Payment");
         break;
      default:
         break;
   }
}

int main() {
    
   DDRA = 0x00; PORTA = 0xFF;
   DDRC = 0xFF; PORTC = 0x00;
   DDRD = 0xC0; PORTD = 0x3F;
   DDRB = 0xFF; PORTB = 0x00;
   
  unsigned char i=0;
  tasks[i].state = CH_SMStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Checkout;  
  i++;
  tasks[i].state = SC_SMStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Scanner;  
  i++;
  tasks[i].state = GP_SMStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Stock;
  i++;
  tasks[i].state = PAY_SMStart;
  tasks[i].period = 100;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Receipt;  

   TimerSet(10);
   TimerOn();
   LCD_init();
   LCD_Cursor(1);
    
   while(1) {
   		for ( i = 0; i < tasksNum; i++ ) {
   			if ( tasks[i].elapsedTime == tasks[i].period ) {
   				tasks[i].state = tasks[i].TickFct(tasks[i].state);
   				tasks[i].elapsedTime = 0;
   			}
   			tasks[i].elapsedTime += 1;
   		}
   		
   		while(!TimerFlag);
   		TimerFlag = 0;
   }
   return 0;
}
