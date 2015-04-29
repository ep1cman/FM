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
    else
        __delay_ms(2);
}

void initLCD()
{
    TRISE &= 0b11001111; // Set LCD_E and LCD_RS as outputs
    TRISD =  0b00000000; // Set LCD_DATA as outputs
    
    //Reset LCD
    writeToLCD(0x30, 'c');
    writeToLCD(0x30, 'c');
    writeToLCD(0x30, 'c');
    
    //8-Bit Mode, 2 Lines and 5x7 Dot font
    writeToLCD(0x38, 'c');
    
    //Display ON, cursor OFF, blink OFF
    writeToLCD(CURSOR_OFF, 'c');
    
    //Display Clear
    writeToLCD(CLEAR_LCD, 'c');
    
    //Entry Mode, Increment cursor, Don't shift display
    writeToLCD(0x06, 'c'); 

    //Custom Characters
    //Up Arrow
    writeToLCD(0x40, 'c');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00001110, 'd');
    writeToLCD(0b00010101, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    
    //Down Arrow
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00010101, 'd');
    writeToLCD(0b00001110, 'd');
    writeToLCD(0b00000100, 'd');
    
    //BT Top Left
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00000111, 'd');
    writeToLCD(0b00001111, 'd');
    writeToLCD(0b00011110, 'd');
    writeToLCD(0b00011110, 'd');
    writeToLCD(0b00011010, 'd');
    writeToLCD(0b00011100, 'd');
    writeToLCD(0b00011110, 'd');
    
    //BT Top Right
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00011100, 'd');
    writeToLCD(0b00011110, 'd');
    writeToLCD(0b00001111, 'd');
    writeToLCD(0b00010111, 'd');
    writeToLCD(0b00011011, 'd');
    writeToLCD(0b00010111, 'd');
    writeToLCD(0b00001111, 'd');
    
    //BT Bottom Left
    writeToLCD(0b00011100, 'd');
    writeToLCD(0b00011010, 'd');
    writeToLCD(0b00011110, 'd');
    writeToLCD(0b00011110, 'd');
    writeToLCD(0b00001111, 'd');
    writeToLCD(0b00000111, 'd');
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00000000, 'd');
    
    //BT Bottom Right
    writeToLCD(0b00010111, 'd');
    writeToLCD(0b00011011, 'd');
    writeToLCD(0b00010111, 'd');
    writeToLCD(0b00001111, 'd');
    writeToLCD(0b00011110, 'd');
    writeToLCD(0b00011100, 'd');
    writeToLCD(0b00000000, 'd');   
    writeToLCD(0b00000000, 'd');
    
    //Record Circle
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00001110, 'd');
    writeToLCD(0b00011111, 'd');
    writeToLCD(0b00011111, 'd');
    writeToLCD(0b00011111, 'd');
    writeToLCD(0b00001110, 'd');
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00000000, 'd');

    //Tick
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00000001, 'd');
    writeToLCD(0b00000011, 'd');
    writeToLCD(0b00010110, 'd');
    writeToLCD(0b00011100, 'd');
    writeToLCD(0b00001000, 'd');
    writeToLCD(0b00000000, 'd');
    
    /*//Standby Icon
    writeToLCD(0b00000000, 'd');
    writeToLCD(0b00000100, 'd');
    writeToLCD(0b00010101, 'd');
    writeToLCD(0b00010101, 'd');
    writeToLCD(0b00010001, 'd');
    writeToLCD(0b00001110, 'd');
    writeToLCD(0b00000000, 'd');*/
}

void printLCD(char *string)
{
    while(*string)
       writeToLCD(*string++, 'd');
}

void setCursorLocation(unsigned char position, unsigned char line)
{
    if (line) position += 0x40;
    writeToLCD(0x80 + position, 'c');
}

