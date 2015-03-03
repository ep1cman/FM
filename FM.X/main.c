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

/* Function Declaration */
int LCD();

int main(int argc, char** argv) 
{
    int LCD();
    return (EXIT_SUCCESS);
}

int LCD()
{   
    int ADD = 0b10000000;  /* Intial Segment Address */
    int DATA[] = {/* load output data here */};
    int LCD_Address(int sn);
    int LCD_Data(int sn);

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
    
    for (int sn = 0 ; sn < 32 ; sn++)
    {
    ADD = LCD_Address( sn , &ADD );
    LCD_Data(sn , &DATA);
    }

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