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

#include <xc.h>
#include <delays.h>
#define F_CPU 8E6
#define LCD_E LATF0
#define LCD_RS LATF1

/* Variable Declaration */
unsigned int DATA;              /* Data for output */
unsigned int DATA1[16];         /* Display line 1 storage */
unsigned int DATA2[16];         /* Display line 2 storage */
unsigned int line;              /* Display line indicator */

void pause()
{
    Delay100TCYx(1000);
}


void Config_LCD(unsigned int DATA) /* Write instructions */
{
    /* Clear ports */
    LCD_E = 0;
    LCD_RS = 0;
    LATD = 0x00;

    /* Configure outputs */
    TRISF0 = 0;
    TRISF1 = 0;
    TRISD = 0x00;

    /* Write to display */
    LCD_RS = 0;
    LCD_E = 1;
    LATD = DATA;
   
    pause();
    LCD_E = 0;

}

void Write_LCD(unsigned int DATA) /* Write data */
{
    /* Clear ports */
    LCD_E = 0;
    LCD_RS = 0;
    LATD = 0x00;

    /* Configure outputs */
    TRISF0 = 0;
    TRISF1 = 0;
    TRISD = 0x00;

    /* Write to display */ 
    LCD_RS = 1;
    LCD_E = 1;
    LATD = DATA;
    pause();
    LCD_E = 0;

}

void Init_LCD()
{  /* wake up */
    pause();
    DATA = 0b00110000;
    Config_LCD(DATA);

    pause();
    DATA = 0b00110000;
    Config_LCD(DATA);

    pause();
    DATA = 0b00110000;
    Config_LCD(DATA);

    /* Function Set */
    pause();
    DATA = 0b00111000;
    Config_LCD(DATA);

    /* Display Off */
    pause();
    DATA = 0b00001000;
    Config_LCD(DATA);

    /* Display Clear */
    pause();
    DATA = 0b00000001;
    Config_LCD(DATA);

    /* Entry Mode Set */
    pause();
    DATA = 0b00000110;
    Config_LCD(DATA);
}

void ADD_Select(int line)
{    if(line==1)
    {
        DATA = 0x80;
    }
    else if(line==2)
    {
        DATA = 0x80+0x40;
    }
    else
    {
        DATA = 0x80;
    }

    pause();
    Config_LCD(DATA);
}

void DATA_Select(int line)
{
    ADD_Select(line);
    if(line == 1)
    {
    for (int x = 0; x < 15 ; x++)
    Write_LCD(DATA1[x]);
    }

    else if (line == 2)
    {
    for (int x = 0; x < 15 ; x++)
    Write_LCD(DATA2[x]);
    }
}

void LCD()
{
    /* Initialise LCD */
    Init_LCD();

    /* Display On*/
    pause();
    DATA = 0b00001100;
    Config_LCD(DATA);

    DATA1[0] = 0b01001000; /* H */
    DATA1[1] = 0b01100101; /* e */
    DATA1[2] = 0b01101100; /* l */
    DATA1[3] = 0b01101100; /* l */
    DATA1[4] = 0b01101111; /* o */
    DATA1[5] = 0b00100000; /*   */
    DATA1[6] = 0b01010111; /* W */
    DATA1[7] = 0b01101111; /* o */
    DATA1[8] = 0b01110010; /* r */
    DATA1[9] = 0b01101100; /* l */
    DATA1[10] = 0b01100100; /* d */
    DATA1[11] = 0b00100001; /* ! */
    DATA1[12] = 0b00100001; /* ! */
    DATA1[13] = 0b00100001; /* ! */
    DATA1[14] = 0b00100001; /* ! */
    DATA1[15] = 0b00100001; /* ! */

    DATA2[0] = 0b01001010; /* * */
    DATA2[1] = 0b01001010; /* * */
    DATA2[2] = 0b01001010; /* * */
    DATA2[3] = 0b01001010; /* * */
    DATA2[4] = 0b01001010; /* * */
    DATA2[5] = 0b01001010; /* * */
    DATA2[6] = 0b01001010; /* * */
    DATA2[7] = 0b01001010; /* * */
    DATA2[8] = 0b01001010; /* * */
    DATA2[9] = 0b01001010; /* * */
    DATA2[10] = 0b01001010; /* * */
    DATA2[11] = 0b01001010; /* * */
    DATA2[12] = 0b01001010; /* * */
    DATA2[13] = 0b01001010; /* * */
    DATA2[14] = 0b01001010; /* * */
    DATA2[15] = 0b01001010; /* * */

    DATA_Select(1);
    DATA_Select(2);
    while(1);
}

int main(int argc, char** argv)
{
    LCD();
    return (EXIT_SUCCESS);
}
