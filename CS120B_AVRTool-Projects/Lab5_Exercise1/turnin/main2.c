/*	Author: Zachary Hill
 *  Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Lab 5  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States { SM_Start, SM_INIT, SM_INCR, SM_DECR, SM_WAIT, SM_RESET } state;
unsigned char tempA = 0x00;
unsigned char tempC = 0x07;

void TickFct() {
	switch(state) {
		case SM_Start:
			state = SM_INIT;
			break;
		case SM_INIT:
			if((tempA & 0x03) == 0x01) {
				state = SM_INCR;
			}
			else if((tempA & 0x03) == 0x02) {
                                state = SM_DECR;
                        }
			else if((tempA & 0x03) == 0x03) {
                                state = SM_RESET;
                        }
			else if((tempA & 0x03) == 0x00) {
                                state = SM_INIT;
                        }
			break;
		case SM_INCR:
			state = SM_WAIT;
			break;
		case SM_DECR:
                        state = SM_WAIT;
               		break;
		case SM_WAIT:
                        if((tempA & 0x03) == 0x01 || (tempA & 0x03) == 0x02) {
                                state = SM_WAIT;
                        }
                        else if((tempA & 0x03) == 0x03) {
                                state = SM_RESET;
                        }
                        else if((tempA & 0x03) == 0x00) {
                                state = SM_INIT;
                        }
                        break;
		case SM_RESET:
                        if((tempA & 0x03) == 0x01 || (tempA & 0x03) == 0x02 || (tempA & 0x03) == 0x03) {
                                state = SM_RESET;
                        }
                        else if((tempA & 0x03) == 0x00) {
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
			if(tempC < 0x09) {
				tempC = tempC + 0x01;
			}
			else {
				tempC = 0x09;
			}
			break;
		case SM_DECR:
			if(tempC < 0x00) {
                                tempC = tempC - 0x01;
                        }
                        else {
                                tempC = 0x00;
                        }
                        break;
		case SM_WAIT:
			break;
                case SM_RESET:
			tempC = 0x00;
                        break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;

	state = SM_Start;
    /* Insert your solution below */
    while (1) {
	tempA = ~PINA & 0x03;
	TickFct();
	PORTC = tempC;
    }
    return 1;
}
