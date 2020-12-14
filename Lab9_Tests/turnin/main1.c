/*	Partner(s) Name: none
 *	Lab Section: 23
 *	Assignment: Lab 9  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
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

enum States{Start, INIT, ON, OFF} state;
unsigned char tempA = 0x00;

void Tick_Fct() {
	switch(state) {
		case Start:
			state = INIT;
			break;
		case INIT:
			if(tempA != 0x00)
				state = ON;
			else if (tempA == 0x00)
				state = INIT;
			break;
		case ON:
			if(tempA == 0x01 || tempA == 0x02 || tempA == 0x04)
                                state = ON;
			//turn speaker to OFF state if two or more buttons are pressed simultaneously
			else if(tempA == 0x03 || tempA == 0x05 || tempA == 0x06 || tempA == 0x07)
                                state = OFF;
			else if (tempA == 0x00)
				state = OFF;
			break;
		case OFF:
			state = INIT;
			break;
		default:
			state = Start;
			break;
	}
	switch(state) {
		case Start:
			break;
		case INIT:
			set_PWM(0);
			break;
		case ON:
			if (tempA == 0x01)
				set_PWM(261.63);
			else if (tempA == 0x02)
                                set_PWM(293.66);
			else if (tempA == 0x04)
                                set_PWM(329.63);
			break;
		case OFF:
			set_PWM(0);
			break;
		default:
			break;
	}
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    state = Start;
    PWM_on();
    while (1) {
	tempA = ~PINA & 0x07;
	Tick_Fct();
    }
    return 1;
}
