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
int DATA;              /* Data for output */
int DATA1[16];         /* Display line 1 storage */
int DATA2[16];         /* Display line 2 storage */
int line;              /* Display line indicator */
int Segment;           /* Display segment indicator */

void pause()
{
    Delay100TCYx(120);
}

void enable()
{
    LCD_E = 1;
    pause();
    LCD_E = 0;
}

void Config_LCD(int DATA) /* Write instructions */
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
    LATD = DATA;
    enable();    
}

void Write_LCD(int DATA) /* Write data */
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
    LATD = DATA;
    enable();
}

void Init_LCD()
{
    pause();
    LATD = 0b00110000;
    Config_LCD(DATA);

    pause();
    LATD = 0b00110000;
    Config_LCD(DATA);

    pause();
    LATD = 0b00001000;
    Config_LCD(DATA);

    pause();
    LATD =  0b00000001;
    Config_LCD(DATA);

    pause();
    LATD = 0b00000110;
    Config_LCD(DATA);

    pause();
    LATD=0b00000110;
    Config_LCD(DATA);

    pause();
    LATD=0b00111000;
    Config_LCD(DATA);

    pause();
    LATD=0b00010100;
    Config_LCD(DATA);

    pause();
    LATD=0b00001100;
    Config_LCD(DATA);

    pause();
    LATD=0b00000010;
    Config_LCD(DATA);
}

void Address_Select(int line)
{
    if (line == 1)
    {
    DATA = 0b10000000; /* Set initial address*/
    Config_LCD(DATA);  /* Output address to display */
    }
    else if (line == 2)
    {
    DATA = 0b11000000; /* Set new address*/
    Config_LCD(DATA);  /* Output address to display */
    }
}

void Data_Select()
{
    line = 1; /* temp for testing, line to be determined by menu structure */
    if (line == 1)
    {
    Address_Select(line);
    for (int x = 0; x < 16; x++)
        {
        DATA = DATA1[Segment]; /* Load data */
        Write_LCD(DATA); /* Write data */
        Segment = Segment + 1; /* Go to next segment */
        }
    }
    
    line = 2; /* temp for testing, line to be determined by menu structure */
    if (line == 2)
    {
    Address_Select(line);
    for (int x = 0; x < 16; x++)
        {
        DATA = DATA2[Segment]; /* Load data */
        Write_LCD(DATA); /* Write data */
        Segment = Segment + 1; /* Go to next segment */
        }
    }
}

void Load_Data()
{
    /* Line one data */
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

    /* Line two data */
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
}

void LCD()
{
    /* Initialise LCD */
    Init_LCD();

    /* Function set */
    LCD_E = 0;
    LCD_RS = 0;
    LATD = 0b00111000;
    enable();

    /* Turn on display */
    LCD_E = 0;
    LCD_RS = 0;
    LATD = 0b00001100;
    enable();

    /* Entry mode set */
    LCD_E = 0;
    LCD_RS = 0;
    LATD = 0b00000110;
    enable();

    /* Load data to be output */
    Load_Data();

    /* Select data to be output */
    Data_Select();

    /* Return home */
    LCD_E = 0;
    LCD_RS = 0;
    LATD = 0b00000010;
    enable();
}

int main(int argc, char** argv)
{
    LCD();
    return (EXIT_SUCCESS);
}
