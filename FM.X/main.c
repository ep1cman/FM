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

<<<<<<< HEAD
#define _XTAL_FREQ 8000000

#include "AR1010.h"

// CONFIG7L#

#include <xc.h>

#define _XTAL_FREQ 1000000

#define LCD_RS LATB1
#define LCD_E LATB0
#define LCD_DATA LATD
#define CLEAR_LCD 0x01

void writeToLCD(unsigned char data, char commandOrData)
{
    LCD_DATA = data;

    if(commandOrData == 'c')
        LCD_RS = 0;
    else
        LCD_RS = 1;

    LCD_E = 1;
    LCD_E = 0;
    if(commandOrData == 'c')
        __delay_ms(20);
}

void initLCD()
{
    TRISB &= 0b11111100; // Set LCD_E and LCD_RS as outputs
    TRISD =  0b00000000; // Set LCD_DATA as outputs

    //Reset LCD
    writeToLCD(0x30, 'c');
    writeToLCD(0x30, 'c');
    writeToLCD(0x30, 'c');

    //8-Bit Mode, 2 Lines and 5x7 Dot font
    writeToLCD(0x38, 'c');

    //Display ON, cursor OFF, blink OFF
    writeToLCD(0x0C, 'c');

    //Display Clear
    writeToLCD(CLEAR_LCD, 'c');

    //Entry Mode, Increment cursor, Don't shift display
    writeToLCD(0x06, 'c');
}

void printLCD(unsigned char *string)
{
    while(*string)
       writeToLCD(*string++, 'd');
}

void setCursorLocation(unsigned char position, unsigned char line)
{
    if (line == 2)
        position += 0x40;
    writeToLCD(0x80 + position, 'c');
}

int main(int argc, char** argv) {

  OpenI2C(MASTER, SLEW_OFF);
  TRISC = 0b00011000;
  initAR1010();
  for(int i=0; i<3000; i++){};
  tune(964);

  initLCD();
  printLCD("Hello");
  setCursorLocation(3,2);
  printLCD("World!");

  while(1){};
  return (EXIT_SUCCESS);
}
