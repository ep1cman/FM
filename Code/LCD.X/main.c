/* 
 * File:   main.c
 * Author: Stuart
 *
 * Created on 24 February 2015, 14:32
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

/*
 * 
 */


int main(int argc, char** argv) {
    /* Enable = F0
       RS = F1 */
    TRISD = 0x00;
    TRISF &= 0b11111100;
    /* Turn on display */
    LATF &= 0b00000000;
    LATD = 0b10000000;
    LATF |= 0b00000001;
    /*Set DDRAM Address*/
    LATF &= 0b00000000;
    LATD = 0b10000000;
    LATF |= 0b00000001;
    /*Set Output data*/
    LATF |= 0b00000010;
    LATD = 0b00100011;
    LATF |= 0b00000011;

    return (EXIT_SUCCESS);
}

