/* 
 * File:   LCD.h
 * Author: Sebastian
 *
 * Created on 24 March 2015, 23:00
 */

#ifndef LCD_H
#define	LCD_H

#include <xc.h>

#define LCD_RS LATB1
#define LCD_E LATB0
#define LCD_DATA LATD
#define CLEAR_LCD 0x01
//TODO: Find a way to not need this in two places
#define _XTAL_FREQ 8000000


void writeToLCD(unsigned char data, char commandOrData);
void initLCD();
void printLCD(char *string);
void setCursorLocation(unsigned char position, unsigned char line);


#endif	/* LCD_H */

