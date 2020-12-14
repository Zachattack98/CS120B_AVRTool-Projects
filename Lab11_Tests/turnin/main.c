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


static task task1, task2, task3;
task *tasks[] = { &task1, &task2, &task3 };
unsigned char htag[3] = {5, 12, 9};
unsigned char i;
unsigned char position = 1;

void data(unsigned char row, unsigned char col, unsigned char obst) {
	LCD_Cursor((col+1)+(row-1)*15);
	LCD_WriteData(obst);
}
	

enum PB_states{PB_INIT, PB_WAIT, PB_DISPLAY, PB_RELEASE}PB_state;

int Tick_Button(int PB_state){
        switch(PB_state) {
                case PB_INIT:
                        PB_state = PB_WAIT;
                        break;
                case PB_WAIT:
                        PB_state = ((~PINA & 0x07) == 0x00) ? PB_WAIT : PB_DISPLAY;
                        break;
                case PB_DISPLAY:
                        PB_state = PB_RELEASE;
                        break;
                case PB_RELEASE:
                        PB_state = ((~PINA & 0x07) == 0x00) ? PB_WAIT : PB_RELEASE;
                        break;
                default:
                        PB_state = PB_INIT;
                        break;
        }
        switch(PB_state) {
        case PB_INIT:
                break;
        case PB_WAIT:
                break;
        case PB_DISPLAY:
		if((~PINA & 0x07) == 0x02){
		       if(position == 17)
		       		position = 1;
		}
 		else if((~PINA & 0x07) == 0x01){
			if(position == 1)
				position = 17;		
		}
                break;
        case PB_RELEASE:
                break;
        default:
                break;
        }
        return PB_state;
}



enum GM_states{GM_INIT, GM_WAIT, GM_Start, PLAY, FAIL}GM_state;
const unsigned char string1[32] = "Press start     to proceed      ";
const unsigned char string2[32] = "    Game Over                   ";

int Tick_Game(int GM_state){
	switch(GM_state) {
		case GM_INIT:
			GM_state = GM_WAIT;
			break;
		case GM_WAIT:
			if((~PINA & 0x07) == 0x04)
				GM_state = GM_Start;
			break;
		case GM_Start:
			if((~PINA & 0x07) == 0x00)
				GM_state = PLAY;
			break;
		case PLAY:
			if((~PINA & 0x07) == 0x04)
				GM_state = GM_WAIT;
			for(i=0; i<3; i++){
				if(i<2) {
                                if(position == (htag[i] + 1))
                                        GM_state = FAIL;
                                }
                                else if(i>=2) {
                                if(position == (htag[i] + 16))
                                        GM_state = FAIL;
                                }

				/*if(position == (htag[i] + (i>0)*16)+(i<1)*1)
					GM_state = FAIL;*/	
			}
			break;
		case FAIL:
			if((~PINA & 0x07) == 0x04)
				GM_state = GM_Start;
			break;
		default:
			GM_state = GM_INIT;
			break;
	}
	switch(GM_state) {
	case GM_INIT:
		break;
	case GM_WAIT:
		LCD_DisplayString(1, string1);
		break;
	case GM_Start:
		LCD_ClearScreen();
		break;
	case PLAY:
		break;
	case FAIL:
		LCD_DisplayString(1, string2);
		break;
	default:
		break;
	}
	return GM_state;
}


enum M_states{M_INIT, M_WAIT, MOVE}M_state;

int Tick_Motion(int M_state){
	switch(M_state){
		case M_INIT:
			M_state = M_WAIT;
			break;
		case M_WAIT:
			if(task2.state == PLAY)
				M_state = MOVE;
			break;
		case MOVE:
			if(task2.state == GM_WAIT || task2.state == FAIL)
				M_state = M_WAIT;
			break;
		default:
			M_state = M_INIT;
			break;
	}
	switch(M_state){
		case M_INIT:
			break;
		case M_WAIT:
			break;
		case MOVE:
			for(i=0; i<3; i++){
				if(htag[i] == 0){
					htag[i] = 15;
					//if(htag[i] == 15)
						//data(1,1, ' ');
				}
				else
					htag[i]--;
			}
			for(i=0; i<2; i++){
				data(1, 0, ' ');
				data(1, htag[i], '#');
				data(1, htag[i]+1, ' ');
			}
			for(i=2; i<3; i++){
				data(2, htag[i], '#');
				data(2, htag[i]+1, ' ');
			}
			break;
		default:
			break;
	}
	return M_state;
}



int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;	//PC7..4 outputs init 0s, PC3..0 inputs init 1s
    DDRD = 0xFF; PORTD = 0x00;

    //Period for the tasks
    unsigned long int SMTick1_calc = 100;
    unsigned long int SMTick2_calc = 500;
    unsigned long int SMTick3_calc = 300;

    //Calculating GCD
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
    tmpGCD = findGCD(tmpGCD, SMTick3_calc);
    
    //GCD for all tasks
    unsigned long int GCD = tmpGCD;

    //Recalculate GCD periods for scheduler
    unsigned long int SMTick1_period = SMTick1_calc/GCD;
    unsigned long int SMTick2_period = SMTick2_calc/GCD;
    unsigned long int SMTick3_period = SMTick3_calc/GCD;

    //Declare an array of tasks
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    //Task 1
    task1.state = 0;
    task1.period = SMTick1_period;
    task1.elapsedTime = SMTick1_period;
    task1.TickFct = &Tick_Button;

    //Task 2
    task2.state = 0;
    task2.period = SMTick2_period;
    task2.elapsedTime = SMTick2_period;
    task2.TickFct = &Tick_Game;

    //Task 3
    task3.state = 0;
    task3.period = SMTick3_period;
    task3.elapsedTime = SMTick3_period;
    task3.TickFct = &Tick_Motion;

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

	LCD_Cursor(position);
	while (!TimerFlag);
	TimerFlag = 0;
    }
}
