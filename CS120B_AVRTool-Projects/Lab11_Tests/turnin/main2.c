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
#include "keypad.h"
#include "scheduler.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


enum states{Start, MESSAGE}state;
unsigned char column = 16;
unsigned char messageSz = 37;
const unsigned char myString[37] = {'C', 'S', '1', '2', '0', 'B', ' ', 'i', 's', ' ', 'L', 'e', 'g', 'e', 'n', 'd', '.', '.', '.', ' ', 'w', 'a', 'i', 't', ' ', 'f', 'o', 'r', ' ', 'i', 't', ' ', 'D', 'A', 'R', 'Y', '!' };
unsigned char ind = 0;
unsigned char currPos = 0;

int Tick_LCD(int state){
	switch(state) {
		case Start:
			state = MESSAGE;
			break;
		case MESSAGE:
			state = MESSAGE;
			break;
		default:
			state = Start;
			break;
	}
	switch(state) {
		case Start:
			break;
		case MESSAGE:
			ind = currPos;
			for(unsigned char i = 1; i <= column; i++) {
				LCD_Cursor(i);
				if(ind >= (messageSz + 16))
					LCD_WriteData(' ');
				else if(ind <= 15)
					LCD_WriteData(' ');
				else
					LCD_WriteData(myString[ind - 16]);
				ind++;
			}
			currPos++;
			if(currPos == 53) //1 + 37 + 15
				currPos = 0;
			break;
		default:
			break;
	}
	return state;
}


int main(void) {
    DDRA = 0xF0; PORTA = 0x0F;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    //Period for the tasks
    unsigned long int SMTick1_calc = 100;

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
    task1.TickFct = &Tick_LCD;

    //Set timer
    TimerSet(GCD);
    TimerOn();

    LCD_init();

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
