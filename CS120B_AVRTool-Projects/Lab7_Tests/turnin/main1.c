/*	Author: Zachary Hill
 *  Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Lab 7  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"

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


enum States { SM_Start, SM_INIT, SM_INCR, SM_DECR, SM_Wait, SM_Reset } state;
unsigned char i = 0x00;
unsigned char c = '0';

void TickFct() {
	switch(state) {
		case SM_Start:
			LCD_Cursor(1);
			LCD_WriteData('0');
			state = SM_INIT;
			break;
		case SM_INIT:
			if ((~PINA & 0x03) == 0x01) {
			   state = SM_INCR;
			}
			else if ((~PINA & 0x03) == 0x02) {
			   state = SM_DECR;
			}
			else if ((~PINA & 0x03) == 0x03) {
			   state = SM_Reset;
			}		
	     		else if ((~PINA & 0x03) == 0x00) {
			   state = SM_INIT;
			}
                        break;

		case SM_INCR:
			state = SM_Wait;
			break;

		case SM_DECR:
			state = SM_Wait;
			break;

		case SM_Wait:
                     if((~PINA & 0x03) == 0x01) {
                     	state = SM_INCR;
                     }
		     else if ((~PINA & 0x03) == 0x02) {
                        state = SM_DECR;
                     }
                     else if ((~PINA & 0x03) == 0x03) {
                        state = SM_Reset;
                     }
		     else if ((~PINA & 0x03) == 0x00) {
			state = SM_INIT;
		     }
		     else {
			state = SM_Wait;
		     }
                     break;

		case SM_Reset:
                     if((~PINA & 0x03) == 0x01 || (~PINA & 0x03) == 0x02 || (~PINA & 0x03) == 0x03) {
                        state = SM_Reset;
                     }
                     else if ((~PINA & 0x03) == 0x00) {
                        state = SM_INIT;
                     }
                        break;
		default:
                        state = SM_Start;
                        break;
	}

	switch(state) {
		case SM_Start:
			break;
		case SM_INIT:
			break;
		case SM_INCR:
			if (i < 0x09) {
				++i;
			}
			else {
				i = 0x09;
			}
			LCD_Cursor(1);
			LCD_WriteData(i + '0');
			break;
		case SM_DECR:
                        if (i > 0x00) {
                                --i;
                        }
                        else {
                                i = 0x00;
                        }
			LCD_Cursor(1);
                        LCD_WriteData(i + '0');
                        break;
		case SM_Wait:
			/*if (state == SM_INCR && i < 0x09) {
                                i = i + 0x01;
                        }
                        else if (state == SM_INCR && i == 0x09) {
                                i = 0x09;
                        }
			else if (state == SM_DECR && i > 0x00) {
                                i = i - 0x01;
                        }
                        else if (state == SM_DECR && i == 0x00){
                                i = 0x00;
                        }
			LCD_Cursor(1);
                        LCD_WriteData(i + '0');*/
                        break;
		case SM_Reset:
			i = 0x00;
			LCD_Cursor(1);
                        LCD_WriteData(i + '0');
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	LCD_init();
	TimerSet(100);
        TimerOn();
    while (1) {
	TickFct();
	while (!TimerFlag);
	TimerFlag = 0;
    }
}
