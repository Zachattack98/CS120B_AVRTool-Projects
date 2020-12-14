/*	Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Lab 9  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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




void set_PWM(double frequency) {
	static double current_frequency;

	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum States{Start, INIT, INC, DEC, WAIT, TOGGLE} state;
unsigned char tempA = 0x00;
unsigned char ind = 0;
unsigned char top = 7;
unsigned char bottom = 0;
unsigned char click = 0;	//temporary bit after pressing on/off button to change from on to off and off to on.
double arr[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
double temp_arr = 0;

void Tick_Fct() {
	switch(state) {
		case Start:
			state = INIT;
			break;
		case INIT:
			if(tempA == 0x01)
				state = TOGGLE;
			else if(tempA == 0x02)
                                state = INC;
			else if(tempA == 0x04)
                                state = DEC;
			else
                                state = INIT;
			break;
		case INC:
			state = WAIT;
			break;
		case DEC:
			state = WAIT;
			break;
		case WAIT:
			if(tempA == 0x00)
                                state = INIT;
			else
				state = WAIT;
			break;
		case TOGGLE:
			state = WAIT;
			break;
		default:
			state = Start;
			break;
	}
	switch(state) {
		case Start:
			break;
		case INIT:
			break;
		case INC:
			if (ind <= (top - 1))
				ind++;
			else
                                ind = top;
			break;
		case DEC:
			if (ind >= (bottom + 1))
				ind--;
			else
				ind = bottom;
			break;
		case WAIT:
			temp_arr = arr[ind];
			set_PWM(temp_arr);
			break;
		case TOGGLE:
			if(click){
				PWM_off();
				click = 0;
			}
			else if (!click){ //starts here after first button press since click is initialized at zero.
				PWM_on();
				click = 1; //change click
			}
		default:
			break;
	}
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(50);
    TimerOn();
    state = Start;
    while (1) {
	tempA = ~PINA & 0x07;
	Tick_Fct();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
