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

/* Variable Declaration */
int ADD = 0b10000000;  /* Intial Segment Address */
int DATA[32]; /* Output Data */
int sn = 0; /* Segment Number */

/* Function Declaration */
int LCD();
int Load_Data();
int LCD_Address(int sn);
int LCD_Data(int sn);

    
int main(int argc, char** argv) 
{
    int LCD();
    return (EXIT_SUCCESS);
}

int LCD()
{   
    /* Initialise Ports */
    TRISD = 0x00;
    TRISF &= 0b11111100;

    /* Turn on display */
    LATF &= 0b00000000;
    LATD = 0b10000000;
    LATF |= 0b00000001;

    /* Function Set */
    LATF &= 0b00000000;
    LATD = 0b00111000;
    LATF |= 0b00000001;

    Load_Data();
    for (int l = 0 ; l < 32 ; l++)
       {
       ADD = LCD_Address(sn);
       LCD_Data(sn);
       sn++;
       }
}

int Load_Data()
{
    DATA[0] = 0b01001000;
    DATA[1] = 0b01100101;
    DATA[2] = 0b01101100;
    DATA[3] = 0b01101100;
    DATA[4] = 0b01101111;
    DATA[5] = 0b00100000;
    DATA[6] = 0b01010111;
    DATA[7] = 0b01101111;
    DATA[8] = 0b01110010;
    DATA[9] = 0b01101100;
    DATA[10] = 0b01100100;
    DATA[11] = 0b00100001;
    return 0;
}

int LCD_Address(int sn)
{
    /*Set DDRAM Address*/
    LATF &= 0b00000000;
    if (sn >= 16) ADD |= 0b01000000; /* Start new line if top line full */
    LATD = ADD;
    LATF |= 0b00000001;
    ADD++; /* Set next adress */
    return 0;
}

int LCD_Data(int sn)
{
    /* Output data */
    LATF |= 0b00000010;
    LATD = DATA[sn];
    LATF |= 0b00000011;
    return 0;
}