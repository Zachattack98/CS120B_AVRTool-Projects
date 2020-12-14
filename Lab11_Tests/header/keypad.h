#ifndef KEYPAD_H
#define KEYPAD_H

#include "bit.h"

#define KEYPADPORT PORTA
#define KEYPADPIN  PINA
#define ROW1 0
#define ROW2 1
#define ROW3 2
#define ROW4 3
#define COL1 4
#define COL2 5
#define COL3 6
#define COL4 7

unsigned char GetKeypadKey() {
	KEYPADPORT = SetBit(0xFF,COL1,0);
	asm("nop");
	if( GetBit(~KEYPADPIN,ROW1) ) { return '1'; }
	if( GetBit(~KEYPADPIN,ROW2) ) { return '4'; }
	if( GetBit(~KEYPADPIN,ROW3) ) { return '7'; }
	if( GetBit(~KEYPADPIN,ROW4) ) { return '*'; }

	KEYPADPORT = SetBit(0xFF,COL2,0);
        asm("nop");
        if( GetBit(~KEYPADPIN,ROW1) ) { return '2'; }
        if( GetBit(~KEYPADPIN,ROW2) ) { return '5'; }
        if( GetBit(~KEYPADPIN,ROW3) ) { return '8'; }
        if( GetBit(~KEYPADPIN,ROW4) ) { return '0'; }

	KEYPADPORT = SetBit(0xFF,COL3,0);
        asm("nop");
        if( GetBit(~KEYPADPIN,ROW1) ) { return '3'; }
        if( GetBit(~KEYPADPIN,ROW2) ) { return '6'; }
        if( GetBit(~KEYPADPIN,ROW3) ) { return '9'; }
        if( GetBit(~KEYPADPIN,ROW4) ) { return '#'; }

	KEYPADPORT = SetBit(0xFF,COL4,0);
        asm("nop");
        if( GetBit(~KEYPADPIN,ROW1) ) { return 'A'; }
        if( GetBit(~KEYPADPIN,ROW2) ) { return 'B'; }
        if( GetBit(~KEYPADPIN,ROW3) ) { return 'C'; }
        if( GetBit(~KEYPADPIN,ROW4) ) { return 'D'; }

	return '\0';
}

#endif
