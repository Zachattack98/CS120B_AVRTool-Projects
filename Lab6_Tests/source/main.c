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


enum States { SM_Start, SM_LD1, SM_LD2, SM_LD3 } state;
unsigned char tempB = 0x00;

void TickFct() {
	switch(state) {
		case SM_Start:
			state = SM_LD1;
			break;
		case SM_LD1:
			state = SM_LD2;
                        break;
		case SM_LD2:
			state = SM_LD3;
			break;
		case SM_LD3:
                        state = SM_LD1;
                        break;
		default:
                        state = SM_Start;
                        break;
	}

	switch(state) {
		case SM_Start:
			break;
		case SM_LD1:
			tempB = 0x01;
			break;
		case SM_LD2:
			tempB = 0x02;
			break;
		case SM_LD3:
                        tempB = 0x04;
                        break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();
	state = SM_Start;
    while (1) {
	tempB = ~tempB;
	TickFct();
	PORTB = tempB;
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
