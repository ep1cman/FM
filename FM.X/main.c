/* 
 * File:   main.c
 * Author: Sebastian Goscik
 * Project: FM Radio
 *
 * Created on 11 February 2015, 18:39
 */

#include <stdio.h>
#include <stdlib.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config OSC = INTIO7     // Oscillator (External RC oscillator, port function on RA6)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switch Over Mode (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power Up Timer (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Voltage (VBOR set to 2.1V)

// CONFIG2H
#pragma config WDT = ON         // Watchdog Timer (WDT enabled)
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

#include <xc.h>
#include "LCD.h"


int main(int argc, char** argv) {

    initLCD();
    printLCD("Hello");
    setCursorLocation(3,2);
    printLCD("World!");

    while(1){};
    return (EXIT_SUCCESS);
}