/*	Author: Zachary Hill
 *  Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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


enum States { SM_Start, SM_INIT, SM_INCR, SM_DECR, SM_Wait, SM_Reset } state;
unsigned char tempA = 0x00;
unsigned char tempB = 0x07;
unsigned char cnt = 0;

void TickFct() {
	switch(state) {
		case SM_Start:
			state = SM_INIT;
			break;
		case SM_INIT:
			if ((tempA & 0x03) == 0x01) {
			   state = SM_INCR;
			}
			else if ((tempA & 0x03) == 0x02) {
			   state = SM_DECR;
			}
			else if ((tempA & 0x03) == 0x03) {
			   state = SM_Reset;
			}		
	     		else if ((tempA & 0x03) == 0x00) {
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
                     if((tempA & 0x03) == 0x01) {
                     	state = SM_INCR;
                     }
		     else if ((tempA & 0x03) == 0x02) {
                        state = SM_DECR;
                     }
                     else if ((tempA & 0x03) == 0x03) {
                        state = SM_Reset;
                     }
		     else if ((tempA & 0x03) == 0x00) {
			state = SM_INIT;
		     }
                     break;

		case SM_Reset:
                     if(((tempA & 0x03) == 0x01) || ((tempA & 0x03) == 0x02) || ((tempA & 0x03) == 0x03)) {
                        state = SM_Reset;
                     }
                     else if ((tempA & 0x03) == 0x00) {
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
			if (tempB < 0x09) {
				tempB = tempB + 0x01;
			}
			else {
				tempB = 0x09;
			}
			break;
		case SM_DECR:
                        if (tempB > 0x00) {
                                tempB = tempB - 0x01;
                        }
                        else {
                                tempB = 0x00;
                        }
                        break;
		case SM_Wait:
			if (state == SM_INCR && tempB < 0x09) {
                                tempB = tempB + 0x01;
                        }
                        else if (state == SM_INCR && tempB == 0x09) {
                                tempB = 0x09;
                        }
			else if (state == SM_DECR && tempB > 0x00) {
                                tempB = tempB - 0x01;
                        }
                        else if (state == SM_DECR && tempB == 0x00){
                                tempB = 0x00;
                        }
                        break;
		case SM_Reset:
			tempB = 0x00;
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(100);
	TimerOn();
	state = SM_Start;
    while (1) {
	tempA = ~PINA & 0x03;
	TickFct();
	PORTB = tempB;
	while (!TimerFlag);
	TimerFlag = 0;
    }
}
