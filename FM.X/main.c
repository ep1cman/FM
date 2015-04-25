/*
 * File:   main.c
 * Author: Sebastian Goscik
 * Project: FM Radio
 *
 * Created on 11 February 2015, 18:39
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config OSC = INTIO67    // Oscillator (External RC oscillator, port function on RA6)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switch Over Mode (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power Up Timer (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Voltage (VBOR set to 2.1V)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Postscaler (1:32768)

// CONFIG3L

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 Mux (CCP2 input/output is multiplexed with RC1)
#pragma config LPT1OSC = OFF    // Low Power Timer1 Osc enable (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // Master Clear Enable (MCLR pin enabled; RG5 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Overflow Reset (Stack full/underflow will cause Reset)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP = OFF         // Code Protect 00000-03FFF (Program memory block (000000-003FFFh) not code-protected)

// CONFIG7L#

#define _XTAL_FREQ 1000000

#include <plib/timers.h>
#include "AR1010.h"
#include "LCD.h"
#include "DS3231.h"

#define VOL_DOWN 0x01
#define VOL_UP 0x02
#define PRESET_DOWN 0x04
#define PRESET_UP 0x08
#define FREQ_DOWN 0x20
#define FREQ_UP 0x10


int i = 0;
unsigned char Timer0Config;

#define MAX_CHECKS 10	// # checks before a switch is debounced
unsigned char oldDebouncedState;	// Debounced state of the switches
unsigned char State[MAX_CHECKS];	// Array that maintains bounce status
unsigned char Index;	// Pointer into State

unsigned char debounce()
{
    unsigned char i;    
    unsigned char newDebouncedState=0xff;
    for(i=0; i<MAX_CHECKS; i++)
        newDebouncedState &= State[i];
    unsigned char out = newDebouncedState & ~oldDebouncedState;
    oldDebouncedState = newDebouncedState;
    return out;
}





void main(int argc, char** argv) {
    //16bit timer with prescalar=1
    Timer0Config = TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_1 ;
    OpenTimer0(Timer0Config);
    WriteTimer0(0xF000); //Please use HEX. Decimal don't work
    INTCONbits.TMR0IF = 0; //reset Interrupt Flag
    INTCONbits.TMR0IE =1;
    ei();     // This is like fliping the master switch to enable interrupt

    //Initialise I2C
    OpenI2C(MASTER, SLEW_OFF);
    TRISC = 0b00011000;
    TRISB = 0xFF;
    ADCON1 |= 0b1111;
    
    initAR1010();
    initLCD();
    setVolume(12);
   
    char* buffer [10] = {0,0,0,0,0,0,0,0,0,0};
    struct dateTime currentDateTime;
    unsigned char menuChanged = 1;
    enum menu menuState = MAIN_MENU;
    unsigned char preset;
    
    while(1)
    {
        unsigned char buttonState = debounce();
        
        switch(menuState)
        {
            case MAIN_MENU:
                //Redraw the screen edge only if the menu screen has changed
    			if (menuChanged)
    			{
    				setCursorLocation(0, 0);
    				writeToLCD(0x00, 'd'); //Up arrow
                    char freqStr[16];
    				sprintf(freqStr, "   %.1fMhz", ((float)getFrequency())/10);
                    printLCD(freqStr);
    				setCursorLocation(0,1);
    				writeToLCD(0x01, 'd'); //Down arrow
    				printLCD((char *)"<<  >>  P+  P- ");
    				menuChanged = 0;
    			}

			    //Volume buttons
                if (buttonState & VOL_UP) volumeUp();
                else if(buttonState & VOL_DOWN) volumeDown();

				//Preset Changed
				if (buttonState & PRESET_UP || buttonState & PRESET_DOWN)
				{
					if (buttonState & PRESET_UP) preset++;
					else if (buttonState & PRESET_DOWN) preset--;
					if (preset<1) preset = 9;
				    if (preset>9) preset = 1;

				    //Display selected preset
				    setCursorLocation(3,0);
					char presetStr[16];
					sprintf(presetStr, "P%d:%.1fMhz", preset, ((float)getFrequency())/10);
					printLCD(presetStr);
				}

    			//Print the currently frequency
    			if (buttonState & FREQ_UP || buttonState & FREQ_DOWN)
				{
                    if (buttonState & FREQ_UP) tune(getFrequency()+1);
                    if (buttonState & FREQ_DOWN) tune(getFrequency()-1);
                    
    				char freqStr[16];
    				sprintf(freqStr, " %.1fMhz   ", ((float)getFrequency())/10);
                    setCursorLocation(3,0);
                    printLCD(freqStr);
    			} 
                break;
        }
    }
}

void interrupt TimerOverflow()
{
    // If timer interrupt
    if(INTCONbits.TMR0IF == 1)
    {
        //Save button states into circular buffer
        State[Index++]=PORTB;
        if(Index>=MAX_CHECKS)Index=0;
        
        //Reset Timer
        INTCONbits.TMR0IF = 0;
        WriteTimer0(0xFC00); //Please use HEX. Decimal don't work
    }
}
