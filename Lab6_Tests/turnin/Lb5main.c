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

enum States { SM_Start, SM_RESET, SM_NORM, SM_REV, SM_WAIT  } state;
unsigned char tempA = 0x00;
unsigned char tempB = 0x00;
unsigned char cnt = 0;
unsigned char cnt2 = 7;
unsigned char led[7] = {2, 10, 42, 43, 47, 63, 0}; 
unsigned char led2[7] = {0, 63, 61, 53, 21, 20, 16};
void TickFct() {
	switch(state) {
		case SM_Start:
			state = SM_RESET;
			break;
		case SM_RESET:
			if((tempA & 0x01) == 0x01 && cnt2 <= 6) {
                                state = SM_REV;
                        }
			else if((tempA & 0x01) == 0x01) {
                                state = SM_NORM;
                        }
                        else if ((tempA & 0x01) == 0x00) {
                                state = SM_RESET;
                        }
                        break;
		case SM_NORM:
			state = SM_WAIT;
			break;
		case SM_REV:
                        state = SM_WAIT;
                        break;
		case SM_WAIT:
			if((tempA & 0x01) == 0x01) {
                                state = SM_WAIT;
                        }
                        else if ((tempA & 0x01) == 0x00) {
                                state = SM_RESET;
                        }
                        break;
		default:
                        state = SM_Start;
                        break;
	}

	switch(state) {
		case SM_Start:
			break;
		case SM_RESET:
			break;
		case SM_NORM:
			tempB = led[cnt];
			cnt++;
			if(cnt > 6) {
				cnt = 0;
				cnt2--;
			}
                        break;
		case SM_REV:
                        tempB = led2[cnt2];
                        cnt2--;
                        if(cnt2 < 0) {
                                cnt2 = 7;
                        }
                        break;
		case SM_WAIT:
			break;
		default:
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	state = SM_Start;
    /* Insert your solution below */
    while (1) {
	tempA = ~PINA & 0x01;
	TickFct();
	PORTB = tempB;
    }
    return 1;
}
