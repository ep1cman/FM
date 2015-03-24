#include "LCD.h"

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

void printLCD(char *string)
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

