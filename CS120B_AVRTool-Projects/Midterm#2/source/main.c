/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
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

unsigned char ping, quake, amp, cross;

enum ping_states { PInit, PHigh, PLow } ping_state;
static int i;
unsigned char H = 1;
unsigned char L = 9;

void Ping() {
        switch(ping_state) {
                case PInit:
                        ping_state = PHigh;
                        break;
                case PHigh:
			if(i <= H)
				ping_state = PHigh;
			else if(i > H) {
				ping_state = PLow;
				i = 0;
			}
                        break;
		case PLow:
                        if(i <= L)
                                ping_state = PLow;
                        else if(i > L) {
                                ping_state = PHigh;
                                i = 0;
                        }
                        break;
                default:
                        ping_state = PInit;
                        break;
        }
        switch(ping_state) {
                case PInit:
                        ping = 0x00;
                        break;
                case PHigh:
                        ping = 0x01;
			H = 1;
			L = 9;
			i++;
                        break;
		case PLow:
			ping = 0x00;
			H = 1;
			L = 9;
			i++;
			break;
		default:
                        break;
        }
}

enum detect_eq_states { DEQInit, DEQBegin, DEQEnd } detect_eq_state;
static int i;

void Detect_EQ() {
        switch(detect_eq_state) {
                case DEQInit:
                        detect_eq_state = DEQBegin;
                        break;
                case DEQBegin:
			if(((~PINA & 0xF8) == 0x00) && i < 10) {
				detect_eq_state = DEQBegin;
			}
			else if(((~PINA & 0xF8) == 0x00) && i >= 10) {
				detect_eq_state = DEQEnd;
				i = 0;
			}
                        break;
		case DEQEnd:
			if(((~PINA & 0xF8) >> 3) >= 0x00 && ((~PINA & 0xF8) >> 3) <= 0x1F)
			       detect_eq_state = DEQBegin;
			else
			       detect_eq_state = DEQEnd;
			break;	
                default:
                        detect_eq_state = DEQInit;
                        break;
        }
        switch(detect_eq_state) {
                case DEQInit:
                        break;
                case DEQBegin:
                        quake = 0x02;
			i++;
                        break;
		case DEQEnd:
			quake = 0x00;
			break;
                default:
                        break;
        }
}

enum detect_zc_states { DZCInit, Non_Zero, Zero_Cross} detect_zc_state;
static int i,j,k;

void Detect_ZC() {
        switch(detect_zc_state) {
                case DZCInit:
			detect_zc_state = Non_Zero;
                        break;
		case Non_Zero:
			i = ~PINA & 0x07;
			if(i <= 7) {
				detect_zc_state = Non_Zero;
				if((~PINA & 0x07) == (k+1)) {
					j = i + 1;
				}
			}
			if(j > 7) {
				detect_zc_state = Zero_Cross;
			}
			k = ~PINA & 0x07;
			break;
		case Zero_Cross:
			detect_zc_state = Non_Zero;
                        break;
                default:
                        detect_zc_state = DZCInit;
                        break;
        }
        switch(detect_zc_state) {
                case DZCInit:
                        break;
		case Non_Zero:
			cross = 0x00;
			break;
		case Zero_Cross:
			cross = 0x04;
			break;
                default:
                        break;
        }
}

enum detect_max_amp_states { DMAInit, AMP } detect_max_amp_state;

void Detect_Max_Amp() {
        switch(detect_max_amp_state) {
                case DMAInit:
			detect_max_amp_state = AMP;
                        break;
		case AMP:
			break;
                default:
                        detect_max_amp_state = DMAInit;
                        break;
        }
        switch(detect_max_amp_state) {
                case DMAInit:
                        break;
		case AMP:
			amp = (quake == 0x02) ? (~PINA & 0xF8) : 0x00;
			break;
                default:
                        break;
        }
}

enum transmit_states { TInit, Toutput } transmit_state;

void Transmit() {
        switch(transmit_state) {
                case TInit:
                        transmit_state = TInit;
                        break;
                case Toutput:
                        break;
                default:
                        transmit_state = TInit;
                        break;
        }
        switch(transmit_state) {
                case TInit:
                        break;
                case Toutput:
                        PORTB = (((cross | amp) | quake) | ping);
                        break;
                default:
                        break;
        }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(100);
    TimerOn();

    ping_state = PInit;
    detect_eq_state = DMAInit;
    detect_max_amp_state = DMAInit;
    detect_zc_state = DZCInit;
    transmit_state = TInit;

    while (1) {
	Ping();
	Detect_EQ();
	Detect_Max_Amp();
	Detect_ZC();
	Transmit();
	while (!TimerFlag) {}
	TimerFlag = 0;
    }
    return 1;
}
