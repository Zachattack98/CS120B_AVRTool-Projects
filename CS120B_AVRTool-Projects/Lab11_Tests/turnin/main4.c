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
unsigned char tempB = 0x1F;

enum KP_states{INIT, GET_KEY}KP_state;

int Tick_Keypad(int KP_state){
	switch(KP_state) {
		case INIT:
			KP_state = GET_KEY;
			break;
		case GET_KEY:
			KP_state = GET_KEY;
			break;
		default:
			KP_state = INIT;
			break;
	}
	switch(KP_state) {
	case INIT:
		break;
	case GET_KEY:
		key = GetKeypadKey();
		switch(key) {
                case '\0': break; // All 5 LEDs on
                case '1': tempB = 0x01; break; // hex equivalent
                case '2': tempB = 0x02; break;
                case '3': tempB = 0x03; break;
                case '4': tempB = 0x04; break;
                case '5': tempB = 0x05; break;
                case '6': tempB = 0x06; break;
                case '7': tempB = 0x07; break;
                case '8': tempB = 0x08; break;
                case '9': tempB = 0x09; break;
                case 'A': tempB = 0x0A; break;
                case 'B': tempB = 0x0B; break;
                case 'C': tempB = 0x0C; break;
                case 'D': tempB = 0x0D; break;
                case '*': tempB = 0x0E; break;
                case '0': tempB = 0x00; break;
                case '#': tempB = 0x0F; break;
                default: tempB = 0x1B; break; // Should never occur. Middle LED off.
		}
		break;
	default:
		break;
	}
	return KP_state;
}


enum DY_states{D_INIT, WAIT, DISPLAY, RELEASE}DY_state;
unsigned char i = 0x01;

int Tick_Display(int DY_state){
        switch(DY_state) {
                case D_INIT:
                        DY_state = WAIT;
                        break;
		case WAIT:
			DY_state = (key == '\0') ? WAIT : DISPLAY;
			break;
                case DISPLAY:
			DY_state = RELEASE;
                        break;
		case RELEASE:
			DY_state = (key == '\0') ? WAIT : RELEASE;
			break;
                default:
                        DY_state = D_INIT;
                        break;
        }
        switch(DY_state) {
        case D_INIT:
                break;
	case WAIT:
		break;
	case DISPLAY: 
		LCD_Cursor(i);
		if(tempB >= 0x00 && tempB <= 0x09)
        		LCD_WriteData(tempB + '0');
		else if(tempB >= 0x0A && tempB <= 0x0D)
			LCD_WriteData(tempB + 0x37);
		else if(tempB == 0x0E)
			LCD_WriteData(tempB + 0x1C);
		else if(tempB == 0x0F)
			LCD_WriteData(tempB + 0x14);

		if(i < 16)
			i++;
		else
			i = 1;
		break;
	case RELEASE:
		break;
	default:
		break;
	}
	return DY_state;
}



int main(void) {
    DDRA = 0xFF; PORTA = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;	//PC7..4 outputs init 0s, PC3..0 inputs init 1s
    DDRD = 0xFF; PORTD = 0x00;

    //Period for the tasks
    unsigned long int SMTick1_calc = 100;
    unsigned long int SMTick2_calc = 100;

    //Calculating GCD
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
    
    //GCD for all tasks
    unsigned long int GCD = tmpGCD;

    //Recalculate GCD periods for scheduler
    unsigned long int SMTick1_period = SMTick1_calc/GCD;
    unsigned long int SMTick2_period = SMTick2_calc/GCD;

    //Declare an array of tasks
    static task task1, task2;
    task *tasks[] = { &task1, &task2 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    //Task 1
    task1.state = 0;
    task1.period = SMTick1_period;
    task1.elapsedTime = SMTick1_period;
    task1.TickFct = &Tick_Keypad;

    //Task 2
    task2.state = 0;
    task2.period = SMTick2_period;
    task2.elapsedTime = SMTick2_period;
    task2.TickFct = &Tick_Display;

    //Set timer
    TimerSet(GCD);
    TimerOn();

    LCD_init();
    LCD_DisplayString(1,"Congratulations!");

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
