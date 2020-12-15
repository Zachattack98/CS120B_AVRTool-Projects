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

enum States{Start, WAIT, PLAY, DOWN, WAIT2} state;
unsigned char tempA = 0x00;
double notes[5] = {329.63, 329.63, 329.63, 440.00, 261.63}; //, 440.00, 440.00, 523.25, 329.63, 261.63, 261.63, 261.63, 523.25, 440.00, 329.63, 440.00, 523.25, 261.63, 329.63, 261.63};
unsigned char time[5] = {50, 50, 50, 13, 50}; //, 13, 13, 37, 50, 50, 50, 50, 37, 13, 50, 13, 37, 50, 50, 50};
unsigned char down[5] = {10, 10, 10, 5, 10};
unsigned char temp_n = 0x00;
unsigned char i = 0x00;
unsigned char j = 0x00;
unsigned char k = 0x00;
unsigned char d = 0x00;

void Tick_Fct() {
	switch(state) {
		case Start:
			state = WAIT;
			break;
		case WAIT:
			if(tempA == 0x01)
				state = PLAY;
			else
                                state = WAIT;
			break;
		case PLAY:
			if(i <= time[j])
				state = PLAY;
			else if(i > time[j]) {
				d++;
				state = DOWN;
			}
			break;
		case DOWN:
			if(k <= down[d] && d < 5) {
		       		state = DOWN;
		 	}
			else if(k > down[d] && d < 5){
				j++;
				state = PLAY;
			}
			else if(d == 5)
				state = WAIT2;		
			break;
		case WAIT2:
			if(tempA == 0x01)
                                state = WAIT2;
                        else
                                state = WAIT;
                        break;
		default:
			state = Start;
			break;
	}
	switch(state) {
		case Start:
			break;
		case WAIT:
			i = 0;
			j = 0;
			d = 0;
			break;
		case PLAY:
			temp_n = notes[j];
			set_PWM(temp_n);
			i++;
			break;
		case DOWN:
			set_PWM(0);
			k++;
			break;
		case WAIT2:
			break;
		default:
			break;
	}
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    PWM_on();
    TimerSet(50);
    TimerOn();
    state = Start;
    while (1) {
	tempA = ~PINA & 0x01;
	Tick_Fct();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
