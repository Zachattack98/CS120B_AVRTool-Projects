/*	Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab 11  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#include "timer.h"
#include "bit.h"
#include "keypad.h"
#include "scheduler.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


unsigned char key;

enum states{INIT, GET_KEY}state;

int Tick_keypad(int state){
	switch(state) {
		case INIT:
			state = GET_KEY;
			break;
		case GET_KEY:
			break;
		default:
			state = INIT;
			break;
	}
	switch(state) {
	case INIT:
		break;
	case GET_KEY:
		key = GetKeypadKey();
		switch(key) {
                case '\0': PORTB = 0x1F; break; // All 5 LEDs on
                case '1': PORTB = 0x01; break; // hex equivalent
                case '2': PORTB = 0x02; break;
                case '3': PORTB = 0x03; break;
                case '4': PORTB = 0x04; break;
                case '5': PORTB = 0x05; break;
                case '6': PORTB = 0x06; break;
                case '7': PORTB = 0x07; break;
                case '8': PORTB = 0x08; break;
                case '9': PORTB = 0x09; break;
                case 'A': PORTB = 0x0A; break;
                case 'B': PORTB = 0x0B; break;
                case 'C': PORTB = 0x0C; break;
                case 'D': PORTB = 0x0D; break;
                case '*': PORTB = 0x0E; break;
                case '0': PORTB = 0x00; break;
                case '#': PORTB = 0x0F; break;
                default: PORTB = 0x1B; break; // Should never occur. Middle LED off.
		}
	default:
		break;
	}
	return state;
}


int main(void) {
    DDRB = 0xFF; PORTB = 0x00;
    DDRA = 0xF0; PORTA = 0x0F;	//PC7..4 outputs init 0s, PC3..0 inputs init 1s

    //Period for the tasks
    unsigned long int SMTick1_calc = 50;

    //Calculating GCD
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(tmpGCD, SMTick1_calc);
    
    //GCD for all tasks
    unsigned long int GCD = tmpGCD;

    //Recalculate GCD periods for scheduler
    unsigned long int SMTick1_period = SMTick1_calc/GCD;

    //Declare an array of tasks
    static task task1;
    task *tasks[] = { &task1 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    //Task 1
    task1.state = 0;
    task1.period = SMTick1_period;
    task1.elapsedTime = SMTick1_period;
    task1.TickFct = &Tick_keypad;

    //Set timer
    TimerSet(GCD);
    TimerOn();

    //Scheduler for-loop iterator
    unsigned short i;
    while (1) {
	//Scheduler code
	for (i=0; i < numTasks; i++) {
		if(tasks[i]->elapsedTime == tasks[i]->period) {
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += 1;
	}

		while (!TimerFlag);
		TimerFlag = 0;
    }
}
