/* 
 * File:   LCD.h
 * Author: Sebastian
 *
 * Created on 24 March 2015, 23:00
 */

#ifndef LCD_H
#define	LCD_H

#include <xc.h>

#define LCD_RS LATE4
#define LCD_E LATE5
#define LCD_DATA LATD
#define CLEAR_LCD 0x01
#define CURSOR_ON 0x0D
#define CURSOR_OFF 0x0C
//TODO: Find a way to not need this in two places
#define _XTAL_FREQ 1000000


void writeToLCD(unsigned char data, char commandOrData);
void initLCD();
void printLCD(char *string);
void setCursorLocation(unsigned char position, unsigned char line);


#endif	/* LCD_H */

