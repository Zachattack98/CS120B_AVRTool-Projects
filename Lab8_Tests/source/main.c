/*      Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 8  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    ADC_init();
    unsigned short MAX = 0x0C7;
    //unsigned short MIN = 0x038; 
    unsigned short temp_max = 0x000;

    temp_max = (MAX / 8);

    while (1) {
	unsigned short x = ADC;

	if(x <= (3 * temp_max))
		PORTB = 0xFF;
    	else if(x <= (3.7 * temp_max))
                PORTB = 0x7F;
	else if(x <= (4.4 * temp_max))
                PORTB = 0x3F;
	else if(x <= (5.1 * temp_max))
                PORTB = 0x1F;
	else if(x <= (5.8 * temp_max))
                PORTB = 0x0F;
	else if(x <= (6.5 * temp_max))
                PORTB = 0x07;
	else if(x <= (7.2 * temp_max))
                PORTB = 0x03;
	else if(x <= (8 * temp_max))
                PORTB = 0x01;
	/*else if((2 * x) <= MIN)
		PORTB = 0x7F;
	else if(x <= MIN)
		PORTB = 0xFF;*/
    }
    return 1;
}
