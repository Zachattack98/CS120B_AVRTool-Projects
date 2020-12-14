/*  Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Lab 10  Exercise 1
 *	Exercise Description: base LEDS for concurrent synchSMs
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



unsigned char threeLEDs, blinkingLED;

enum TH_States {TH_Start, LD1, LD2, LD3} TH_State;
unsigned short cnt3 = 0x00;

void TickFct_ThreeLEDs() {
	switch(TH_State) {
		case TH_Start:
			TH_State = LD1;
			break;
		case LD1:
			if(cnt3 >= 1000) {
				cnt3 = 0;
				TH_State = LD2;
			}
			else if(cnt3 < 1000)
				TH_State = LD1;
			break;
		case LD2:
                        if(cnt3 >= 1000) {
                                cnt3 = 0;
                                TH_State = LD3;
                        }
                        else if(cnt3 < 1000)
                                TH_State = LD2;
                        break;
		case LD3:
                        if(cnt3 >= 1000) {
                                cnt3 = 0;
                                TH_State = LD1;
                        }
                        else if(cnt3 < 1000) 
                                TH_State = LD3;
                        break;
		default:
			TH_State = TH_Start;
			break;
	}

	switch(TH_State) {
		case TH_Start:
			break;
		case LD1:
			threeLEDs = 0x01;
			cnt3++;
			break;
		case LD2:
			threeLEDs = 0x02;
			cnt3++;
			break;
		case LD3:
			threeLEDs = 0x04;
			cnt3++;
			break;
		default:
			break;
	}
}


enum BL_States {BL_Start, ON, OFF} BL_State;
unsigned short cnt1 = 0x00;

void TickFct_BlinkingLED() {
        switch(BL_State) {
                case BL_Start:
                        BL_State = ON;
                        break;
                case ON:
                        if(cnt1 >= 1000) {
                                cnt1 = 0;
                                BL_State = OFF;
                        }
                        else if(cnt1 < 1000)
                                BL_State = ON;
                        break;
                case OFF:
                        if(cnt1 >= 1000) {
                                cnt1 = 0;
                                BL_State = ON;
                        }
                        else if(cnt1 < 1000)
                                BL_State = OFF;
                        break;
		default:
			BL_State = BL_Start;
			break;
	}

	switch(BL_State) {
		case BL_Start:
			break;
		case ON:
			blinkingLED = 0x08;
			cnt1++;
			break;
		case OFF:
			blinkingLED = 0x00;
			cnt1++;
			break;
		default:
			break;
	}
}


enum CB_States {CB_Start, COMB} CB_State;

void TickFct_CombineLEDs() {
	switch(CB_State) {
		case CB_Start:
			CB_State = COMB;
			break;
		case COMB:
			break;
		default:
			CB_State = CB_Start;
			break;
	}

	switch(CB_State) {
		case CB_Start:
			break;
		case COMB:
			PORTB = threeLEDs | blinkingLED;
			break;
		default:
			break;
	}
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;

    TH_State = TH_Start;
    BL_State = BL_Start;
    CB_State = CB_Start;

    TimerSet(1);
    TimerOn();
    while (1) {
	TickFct_ThreeLEDs();
	TickFct_BlinkingLED();
	TickFct_CombineLEDs();

	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
