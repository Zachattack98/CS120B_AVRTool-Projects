/*	Author: Zachary Hill
 *  Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Lab 7  Exercise 2
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


enum States { SM_Start, SM_LD1, SM_LD2, SM_LD3, SM_Check, SM_Wait, SM_Release, SM_Reset } state;
unsigned char cnt = 0;
unsigned char i = 0;
unsigned char score = 0x00;

void TickFct() {
	switch(state) {
		case SM_Start:
			PORTB = 0x00;
			score = 5;
			//LCD_Cursor(1);
			//LCD_WriteData(score + '0');
			state = SM_LD1;
			break;
		case SM_LD1:
			if ((~PINA & 0x01) == 0x01 && cnt < 3) {
			   state = SM_Check;
			   cnt = 0;
			}
			else if ((~PINA & 0x01) == 0x00 && cnt < 3) {
			   state = SM_LD1;
			   cnt++;
			}
			else if ((~PINA & 0x01) == 0x00 && cnt == 3) {
			   state = SM_LD2;
			   cnt = 0;
			}		
                        break;

		case SM_LD2:
                        if ((~PINA & 0x01) == 0x01 && cnt < 3) {
                           state = SM_Check;
                           cnt = 0;
                        }
                        else if ((~PINA & 0x01) == 0x00 && cnt < 3) {
                           state = SM_LD2;
                           cnt++;
                        }
                        else if ((~PINA & 0x01) == 0x00 && cnt == 3 && i == 0) {
                           state = SM_LD3;
                           cnt = 0;
                        }
			else if ((~PINA & 0x01) == 0x00 && cnt == 3 && i == 1) {
                           state = SM_LD1;
                           cnt = 0;
			   i = 0;
                        }
                        break;

		case SM_LD3:
                        if ((~PINA & 0x01) == 0x01 && cnt < 3) {
                           state = SM_Check;
                           cnt = 0;
                        }
                        else if ((~PINA & 0x01) == 0x00 && cnt < 3) {
                           state = SM_LD3;
                           cnt++;
                        }
                        else if ((~PINA & 0x01) == 0x00 && cnt == 3) {
                           state = SM_LD2;
                           cnt = 0;
			   i++;
                        }
                        break;

		case SM_Check:
			state = SM_Wait;
			break;

		case SM_Wait:
			if((~PINA & 0x01) == 0x01) {
				state = SM_Wait;
			}
			else {
				state = SM_Release;
			}
                     break;

		case SM_Release:
                        if((~PINA & 0x01) == 0x01)
{
                                state = SM_Reset;
                        }
                        else {
                                state = SM_Release;
                        }
                        break;

		case SM_Reset:
                     if((~PINA & 0x03) == 0x01 || (~PINA & 0x03) == 0x02 || (~PINA & 0x03) == 0x03) {
                        state = SM_Reset;
                     }
                     else if ((~PINA & 0x03) == 0x00) {
                        if(score >= 9) {
				score = 5;
				//LCD_ClearScreen();
				//LCD_Cursor(1);
				//LCD_WriteData(score + '0');
			}
			state = SM_LD1;
                     }
                        break;
		default:
                        state = SM_Start;
                        break;
	}

	switch(state) {
		case SM_Start:
			break;
		case SM_LD1:
			PORTB = 0x01;
			break;
		case SM_LD2:
                        PORTB = 0x02;
                        break;
		case SM_LD3:
                        PORTB = 0x04;
                        break;
		case SM_Check:
			if(PORTB == 0x02) {
				++score;
				if(score >= 9) {
					//LCD_DisplayString(1, "WINNER!");
				}
				else {
					//LCD_Cursor(1);
					//LCD_WriteData(score + '0');
				}
			}
			else {
				--score;
				if(score <= 0) {
					score = 0;
				}
				//LCD_Cursor(1);
				//LCD_WriteData(score + '0');
			}
			break;
		case SM_Wait:
                        break;
		case SM_Release:
			break;
		case SM_Reset:
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
	LCD_init();
	TimerSet(100);
        TimerOn();

	LCD_Cursor(1);
        LCD_WriteData(score + '0');
    while (1) {
	TickFct();
	while (!TimerFlag);
	TimerFlag = 0;
    }
}
