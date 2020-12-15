/*	Author: Zachary Hill
 *  Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Project 1
 *	Exercise Description: Fishing Simulation
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
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}
void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

//functions to start task scheduling
typedef struct task {
	int state;
        unsigned long period;
	unsigned long elapsedTime;
        int (*TickFct)(int);
} task;

task tasks[4];
const unsigned short tasksNum = 4;


enum Fish_States { F_SMStart, F_Init, F_Roulette} F_State;
unsigned char fish_id = 0;

void TickFct_Locate() {
	switch(F_State) {
		case F_SMStart:
			F_State = F_Init;
			break;
		case F_Init:
			if((~PINA & 0x0F) == 0x01) {
				F_State = F_Roulette;
				fish_id = (rand() % 10) + 1;
			}
			else if((~PINA & 0x0F) == 0x00) {
				F_State = F_Init;
			}
			break;
		case F_Roulette:
			if((~PINA & 0x0F) == 0x01) {
				F_State = F_Roulette;
				fish_id = (rand() % 10) + 1;
			}
			else if((~PINA & 0x0F) == 0x00) {
				F_State = F_Init;
			}
		default:
			F_State = F_SMStart;
			break;
	}

	switch (F_State) {
		case F_SMStart:
			break;
		case F_Init:
			break;
		case F_Roulette:
			break;
		default:
			break;
	}
}

enum Radar_States {Rd_SMStart, Rd_Wave1On, Rd_Wave2On, Rd_Wave3On, Rd_Reset} Rd_State;

void TickFct_Wave() {
	switch (Rd_State) {
		case Rd_SMStart:
			Rd_State = Rd_Wave1On;
			break;
		case Rd_Wave1On:
		if((~PINA & 0x0F) == 0x01) {
			Rd_State = Rd_Wave2On;
		}
		else if ((~PINA & 0x0F) == 0x03) {
			Rd_State = Rd_Reset;
		}
		else if ((~PINA & 0x0F) == 0x00) {
			Rd_State = Rd_Wave1On;
		}
			break;
		case Rd_Wave2On:
		if((~PINA & 0x0F) == 0x01) {
			Rd_State = Rd_Wave3On;
		}
		else if ((~PINA & 0x0F) == 0x03) {
			Rd_State = Rd_Reset;
		}
		else if ((~PINA & 0x0F) == 0x00) {
			Rd_State = Rd_Wave1On;
		}
			break;
		case Rd_Wave3On:
		if((~PINA & 0x0F) == 0x01) {
			Rd_State = Rd_Wave1On;
		}
		else if ((~PINA & 0x0F) == 0x03) {
			Rd_State = Rd_Reset;
		}
		else if ((~PINA & 0x0F) == 0x00) {
			Rd_State = Rd_Wave1On;
		}
			break;
                case Rd_Reset:
		if((~PINA & 0x0F) == 0x05) {
			Rd_State = Rd_Wave1On;
		}
		else if ((~PINA & 0x0F) == 0x01) {
			Rd_State = Rd_Reset;
		}
		else if ((~PINA & 0x0F) == 0x00) {
			Rd_State = Rd_Wave1On;
		}
			break;
		default:
			Rd_State = Rd_SMStart;
			break;
	}

	switch (Rd_State) {
		case Rd_SMStart:
			break;
		case Rd_Wave1On:
		if ((~PINA & 0x0F) == 0x01) {
			PORTB = 0x01;
		}
		else if ((~PINA & 0x0F) == 0x00) {
			PORTB = 0x00;
		}
			break;
		case Rd_Wave2On:
			PORTB = 0x02;
			break;
		case Rd_Wave3On:
			PORTB = 0x04;
			break;
		case Rd_Reset:
			PORTB = 0x00;
			break;
		default:
			break;
	}
}

enum Message_States {Mg_SMStart, Mg_Intro, Mg_LMBass, Mg_LNDace, Mg_RNBTrout, Mg_ATLSalmon, Mg_Boot, Mg_Fail} Mg_State;
unsigned char pass = 0;

void TickFct_Name() {
	switch (Mg_State) {
		case Mg_SMStart:
			Mg_State = Mg_Intro;
			break;
		case Mg_Intro:
			if ((~PINA & 0x0F) == 0x00 || (~PINA & 0x0F) == 0x01) {
				Mg_State = Mg_Intro;
			}
			else if ((~PINA & 0x0F) == 0x03 && fish_id == 6) {
				Mg_State = Mg_LMBass;
			}
			else if ((~PINA & 0x0F) == 0x03 && fish_id == 7) {
				Mg_State = Mg_LNDace;
			}
			else if ((~PINA & 0x0F) == 0x03 && fish_id == 8) {
				Mg_State = Mg_RNBTrout;
			}
			else if ((~PINA & 0x0F) == 0x03 && fish_id == 9) {
				Mg_State = Mg_ATLSalmon;
			}
			else if ((~PINA & 0x0F) == 0x03 && fish_id == 10) {
				Mg_State = Mg_Boot;
			}
			else if ((~PINA & 0x0F) == 0x03){
				Mg_State = Mg_Fail;
			}
			break;
		case Mg_LMBass:
			if((~PINA & 0x0F) == 0x05) {
				Mg_State = Mg_Intro;
			}
			else {
				Mg_State = Mg_LMBass;
			}
			break;
		case Mg_LNDace:
			if((~PINA & 0x0F) == 0x05) {
				Mg_State = Mg_Intro;
			}
			else {
				Mg_State = Mg_LNDace;
			}
			break;
		case Mg_RNBTrout:
			if((~PINA & 0x0F) == 0x05) {
				Mg_State = Mg_Intro;
			}
			else {
				Mg_State = Mg_RNBTrout;
			}
			break;
		case Mg_ATLSalmon:
			if((~PINA & 0x0F) == 0x05) {
				Mg_State = Mg_Intro;
			}
			else {
				Mg_State = Mg_ATLSalmon;
			}
			break;
		case Mg_Boot:
			if((~PINA & 0x0F) == 0x05) {
				Mg_State = Mg_Intro;
			}
			else {
				Mg_State = Mg_Boot;
			}
			break;
		case Mg_Fail:
			if((~PINA & 0x0F) == 0x05) {
				Mg_State = Mg_Intro;
			}
			else {
				Mg_State = Mg_Fail;
			}
			break;
		default:
			Mg_State = Mg_SMStart;
			break;
	}

	switch (Mg_State) {
		case Mg_SMStart:
			break;
		case Mg_Intro:
			if((~PINA & 0x0F) == 0x01) {
				LCD_DisplayString(1, "Press  Lock-On  Button");
			}
			else if((~PINA & 0x0F) == 0x00) {
				LCD_ClearScreen();
			}
			pass = 0;
			break;
		case Mg_LMBass:
			LCD_DisplayString(1, "Found LargemouthBass!");
			pass = 1;
			break;
		case Mg_LNDace:
			LCD_DisplayString(1, "Found Longneck  Dace!");
			pass = 1;
			break;
		case Mg_RNBTrout:
			LCD_DisplayString(1, "Found  Rainbow  Trout!");
			pass = 1;
			break;
		case Mg_ATLSalmon:
			LCD_DisplayString(1, "Found Atlantic  Salmon!");
			pass = 1;
			break;
		case Mg_Boot:
			LCD_DisplayString(1, "Ew! Stinky Boot!");
			break;
		case Mg_Fail:
			LCD_DisplayString(1, "Sorry! Missed   the Fish!");
			break;
		default:
			break;
	}
}

enum Catch_States {C_SMStart, C_Init, C_Motion, C_Succeed, C_Fail } C_State;
unsigned char snag = 0;
unsigned char cnt = 0;

void TickFct_Capture() {
	switch ( C_State ) {
		case C_SMStart:
			C_State = C_Init;
			break;
		case C_Init:
			if((~PINA & 0x0F) == 0x01 && pass == 1) {
				C_State = C_Motion;
			}
			else if(pass == 0){
				C_State = C_Init;
			}
			break;
		case C_Motion:
			if((~PINA & 0x0F) == 0x05 || (~PINA & 0x0F) == 0x00) {
				C_State = C_Init;
			}
			else if((~PINA & 0x0F) == 0x09 && snag == 1) {
				C_State = C_Fail;
			}
			else if((~PINA & 0x0F) == 0x09 && snag == 2) {
				C_State = C_Succeed;
			}
			else if ((~PINA & 0x0F) == 0x01) {
				C_State = C_Motion;
			}
			//else if((~PINA & 0x0F) == 0x09) {
				//C_State = C_Succeed;
			//}
			//else {
				//C_State = C_Motion2;
			//}
			break;
		//case C_Motion2:
			//if((~PINA & 0x0F) == 0x05 || (~PINA & 0x0F) == 0x00) {
				//C_State = C_Init;
			//}
			//else if((~PINA & 0x0F) == 0x09) {
				//C_State = C_Fail;
			//}
			//else {
				//C_State = C_Motion1;
			//}
			//break;
		case C_Succeed:
			if ((~PINA & 0x0F) == 0x05 || (~PINA & 0x0F) == 0x00) {
				C_State = C_Init;
			}
			else if ((~PINA & 0x0F) == 0x01){
				C_State = C_Succeed;
			}
			break;
		case C_Fail:
			if ((~PINA & 0x0F) == 0x05 || (~PINA & 0x0F) == 0x00) {
				C_State = C_Init;
			}
			else if ((~PINA & 0x0F) == 0x01){
				C_State = C_Fail;
			}
			break;
		default:
			C_State = C_SMStart;
			break;
	}

	switch (C_State) {
		case C_SMStart:
			break;
		case C_Init:
			if (pass == 1) {
				PORTB = 0x00;
			}
			//snag = 0;
			break;
		case C_Motion:
			//if((~PINA & 0x0F) == 0x09) {
				//snag = 1;
			//}
			//else if ((~PINA & 0x0F) == 0x01) {
				//snag = 0;
			//}
			cnt++;

			if (cnt >= 101) {
				cnt = 1;
			}

			if (cnt % 2 == 1) {
				snag = 1;
				PORTB = 0x10;
			}
			else if (cnt % 2 == 0) {
				snag = 2;
				PORTB = 0x00;
			}
			break;
		//case C_Motion2:
			//if((~PINA & 0x0F) == 0x09) {
				//snag = 1;
			//}
			//else if ((~PINA & 0x0F) == 0x01) {
				//snag = 0;
			//}
			//PORTB = 0x00;
			//break;
		case C_Succeed:
			LCD_DisplayString(1, "Capture Success!");
			break;
		case C_Fail:
			LCD_DisplayString(1, "Capture Failed!");
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	unsigned char i = 0;
	tasks[i].state = F_SMStart;
	tasks[i].period = 50;
	tasks[i].elapsedTime = tasks[i].period;
        tasks[i].TickFct = &TickFct_Locate;
	i++;
	tasks[i].state = Rd_SMStart;
	tasks[i].period = 50;
	tasks[i].elapsedTime = tasks[i].period;
        tasks[i].TickFct = &TickFct_Wave;
	i++;
	tasks[i].state = Mg_SMStart;
	tasks[i].period = 50;
	tasks[i].elapsedTime = tasks[i].period;
        tasks[i].TickFct = &TickFct_Name;
	i++;
	tasks[i].state = C_SMStart;
	tasks[i].period = 150;
	tasks[i].elapsedTime = tasks[i].period;
        tasks[i].TickFct = &TickFct_Capture;

	TimerSet(10);
        TimerOn();
        LCD_init();
	LCD_Cursor(1);

    while (1) {
	for (i=0; i<tasksNum; i++) {
		if(tasks[i].elapsedTime == tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += 1;
	}

	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
