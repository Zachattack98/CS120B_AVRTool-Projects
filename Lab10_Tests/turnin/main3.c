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



unsigned char threeLEDs, blinkingLED, noise;

enum TH_States {TH_Start, LD1, LD2, LD3} TH_State;
//unsigned short cnt3 = 0x00;

void TickFct_ThreeLEDs() {
	switch(TH_State) {
		case TH_Start:
			TH_State = LD1;
			break;
		case LD1:
			//if(cnt3 >= 1000) {
				//cnt3 = 0;
				TH_State = LD2;
			//}
			//else if(cnt3 < 1000)
				//TH_State = LD1;
			break;
		case LD2:
                        //if(cnt3 >= 1000) {
                                //cnt3 = 0;
                                TH_State = LD3;
                        //}
                        //else if(cnt3 < 1000)
                                //TH_State = LD2;
                        break;
		case LD3:
                        //if(cnt3 >= 1000) {
                                //cnt3 = 0;
                                TH_State = LD1;
                        //}
                        //else if(cnt3 < 1000) 
                                //TH_State = LD3;
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
			//cnt3++;
			break;
		case LD2:
			threeLEDs = 0x02;
			//cnt3++;
			break;
		case LD3:
			threeLEDs = 0x04;
			//cnt3++;
			break;
		default:
			break;
	}
}


enum BL_States {BL_Start, ON, OFF} BL_State;

void TickFct_BlinkingLED() {
        switch(BL_State) {
                case BL_Start:
                        BL_State = ON;
                        break;
                case ON:
                        BL_State = OFF;
                        break;
                case OFF:
                        BL_State = ON;
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
			break;
		case OFF:
			blinkingLED = 0x00;
			break;
		default:
			break;
	}
}


enum SP_States {SP_Start, WAIT, ON_SWITCH, OFF_SWITCH} SP_State;
unsigned short cntsp = 0x00;

void TickFct_Speaker() {
        switch(SP_State) {
                case SP_Start:
                        SP_State = WAIT;
                        break;
		case WAIT:
			if((~PINA & 0x04) == 0x04)
				SP_State = ON_SWITCH;
			else
				SP_State = WAIT;
			break;
                case ON_SWITCH:
			if((cntsp < 2) && (~PINA & 0x04) == 0x04) {
				SP_State = ON_SWITCH;
			}
			else if((~PINA & 0x04) == 0x04) {
                        	SP_State = OFF_SWITCH;
				cntsp = 0;
			}
			else
				SP_State = WAIT;
                        break;
                case OFF_SWITCH:
                        if((cntsp < 2) && (~PINA & 0x04) == 0x04) {
                                SP_State = OFF_SWITCH;
			}
                        else if((~PINA & 0x04) == 0x04) {
                                SP_State = ON_SWITCH;
				cntsp = 0;
			}
			else
				SP_State = WAIT;
                        break;
                default:
                        SP_State = SP_Start;
                        break;
        }

        switch(SP_State) {
                case SP_Start:
                        break;
		case WAIT:
			noise = 0x00;
			cntsp = 0x00;
			break;
                case ON_SWITCH:
                        noise = 0x10;
			cntsp++;
                        break;
                case OFF_SWITCH:
                        noise = 0x00;
			cntsp++;
                        break;
                default:
                        break;
	}
}


enum CB_States {CB_Start, COMB} CB_State;
unsigned char tempB = 0x00;

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
			tempB = (threeLEDs | blinkingLED) | noise;
			//tempB = threeLEDs | noise;
			PORTB = tempB;
			break;
		default:
			break;
	}
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    unsigned long TH_elapsedTime = 300;
    unsigned long BL_elapsedTime = 1000;
    //unsigned long SP_elapsedTime = 2;
    const unsigned long timerPeriod = 1;

    TimerSet(timerPeriod);
    TimerOn();

    TH_State = TH_Start;
    BL_State = BL_Start;
    SP_State = SP_Start;
    CB_State = CB_Start;
    while (1) {
	if(TH_elapsedTime >= 300) {
		TickFct_ThreeLEDs();
		TH_elapsedTime = 0;
	}
	if(BL_elapsedTime >= 1000) {
		TickFct_BlinkingLED();
		BL_elapsedTime = 0;
	}
	//if(SP_elapsedTime >= 2) {
		TickFct_Speaker();
		//SP_elapsedTime = 0;
	//}
	TickFct_CombineLEDs();

	while(!TimerFlag);
	TimerFlag = 0;
	TH_elapsedTime += timerPeriod;
	BL_elapsedTime += timerPeriod;
	//SP_elapsedTime += timerPeriod;
    }
    return 1;
}
