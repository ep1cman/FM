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
#pragma config OSC = INTIO67       // Oscillator (External RC oscillator, port function on RA6)
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

#define _XTAL_FREQ 8000000

#include <plib/timers.h>
#include "AR1010.h"
#include "LCD.h"
#include "DS3231.h"

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
    for(i=0; i<MAX_CHECKS-newDebouncedState; i++)
        newDebouncedState &= State[i];
    unsigned char out = ~newDebouncedState & oldDebouncedState;
    oldDebouncedState = newDebouncedState;
    return out;
}

void main(int argc, char** argv) {
    // Internal Clock to 8MHz
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;

    //16bit timer with prescalar=1
    Timer0Config = TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_1 ;
    OpenTimer0(Timer0Config);
    WriteTimer0(0xF800); //Please use HEX. Decimal don't work
    INTCONbits.TMR0IF = 0; //reset Interrupt Flag
    ei();     // This is like fliping the master switch to enable interrupt

    //Initialise I2C
    OpenI2C(MASTER, SLEW_OFF);
    TRISC = 0b00011000;

    TRISF = 0xFF; //set port F as all inputs
  
    initAR1010();
    initLCD();

    tune(964);

    char* buffer [10] = {0,0,0,0,0,0,0,0,0,0};
    struct dateTime currentDateTime;

    while(1)
    {
        getDateTime(&currentDateTime);
        setCursorLocation(0,1);
        sprintf(buffer, "%02d %02d/%02d/20%02d", currentDateTime.day, currentDateTime.date, currentDateTime.month, currentDateTime.year);
        printLCD(buffer);
        setCursorLocation(0,2);
        sprintf(buffer, "%02d:%02d:%02d", currentDateTime.hour, currentDateTime.min, currentDateTime.sec);
        printLCD(buffer);
        
        printLCD(" FM: 96.4")
    }
    return (EXIT_SUCCESS);
}

void interrupt TimerOverflow()
{
    // If timer interrupt
    if(INTCONbits.TMR0IF == 1)
    {
        //Save button states into circular buffer
        State[Index++]=PORTF;
        if(Index>=MAX_CHECKS)Index=0;
        
        //Reset Timer
        INTCONbits.TMR0IF = 0;
        WriteTimer0(0xFC00); //Please use HEX. Decimal don't work
    }
}
