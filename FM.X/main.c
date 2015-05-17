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
#pragma config OSC = INTIO67    // Oscillator (External RC oscillator, port function on RA6)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switch Over Mode (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power Up Timer (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown Out Detect (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Voltage (VBOR set to 2.1V)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer (WDT disabled (control is placed on the SWDTEN bit))
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

#define _XTAL_FREQ 1000000

#include <plib/timers.h>
#include "AR1010.h"
#include "LCD.h"
#include "DS3231.h"
#include "MAX4586.h"
#include "EEPROM.h"
#include "Recorder.h"

#define VOL_DOWN 0x01
#define VOL_UP 0x02
#define PRESET_DOWN 0x20
#define PRESET_UP 0x04
#define FREQ_DOWN 0x10
#define FREQ_UP 0x08


int i = 0;
unsigned char Timer0Config;

#define MAX_CHECKS 10	// # checks before a switch is debounced
unsigned char oldDebouncedState;	// Debounced state of the switches
unsigned char State[MAX_CHECKS];	// Array that maintains bounce status
unsigned char Index;	// Pointer into State

enum menu {MENU, FM, BT, REC, PLAYBACK, PLAYING, OFF};
enum dateSelect {DATE_SEL,MONTH_SEL,HOUR_SEL,MIN_SEL};

unsigned char debounce()
{
    unsigned char i;    
    unsigned char newDebouncedState=0xff;
    for(i=0; i<MAX_CHECKS; i++)
        newDebouncedState &= State[i];
    unsigned char out = newDebouncedState & ~oldDebouncedState;
    oldDebouncedState = newDebouncedState;
    return out;
}

void main(int argc, char** argv) {
    //16bit timer with prescalar=1
    Timer0Config = TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_1 ;
    OpenTimer0(Timer0Config);
    WriteTimer0(0xF000); //Please use HEX. Decimal don't work
    INTCONbits.TMR0IF = 0; //reset Interrupt Flag
    INTCONbits.TMR0IE =1;
    ei();     // This is like fliping the master switch to enable interrupt

    //Initialise I2C
    OpenI2C(MASTER, SLEW_OFF);
    TRISC = 0b00011000;

    //Setup ports, turn off ADC
    LATC |= 0b00000100; //Turn on backlight
    TRISB = 0xFF;
    ADCON1 |= 0b1111;
    
    initAR1010();
    initLCD();
    setVolume(12);
   
    //Variable Declarations
    char* buffer [10] = {0,0,0,0,0,0,0,0,0,0};
    struct dateTime scheduleSelect;
    enum menu menuState = FM;
    enum menu oldMenu = MENU;
    unsigned char preset;
    unsigned char file = 1;
    
    while(1)
    {
        unsigned char buttonState = debounce();
        unsigned char buttonPressed = (buttonState>0);

        // Return to the menu is VOL_UP and VOL_DOWN are pressed
        if ((oldDebouncedState & VOL_UP) && (oldDebouncedState & VOL_DOWN))
        {
            oldMenu = menuState;
            menuState = MENU;
        }
        
        //Menu "state machine"
        switch(menuState)
        {
            case MENU:
                //Turn on LCD Backlight
                LATC |= 0b00000100;
                
                //Draw the menu
                writeToLCD(CLEAR_LCD, 'c');
                printLCD("FM BT REC PB OFF");
                setCursorLocation(0,1);
    			printLCD((char *)"\x7F    \x7E    x    \x07");
                writeToLCD(CURSOR_ON, 'c');
                setCursorLocation(0,0);

                enum menu selection = oldMenu;
                while(menuState == MENU)
                {
                    //Handle inputs
                    buttonState = debounce();
                    if (buttonState & PRESET_DOWN)
                        selection--;
                    else if (buttonState & FREQ_DOWN)
                        selection++;
                    else if (buttonState & FREQ_UP) 
                        menuState = oldMenu;
                    else if (buttonState & PRESET_UP)
                        menuState = selection;
                    
                    //Cycle menu selection when it goes off screen
                    if (selection==MENU) selection = OFF;
                    if (selection>OFF) selection = FM;

                    //Highlight selected option
                    switch (selection)
                    {
                        case FM:
                            setCursorLocation(0,0);
                            break;
                        case BT:
                            setCursorLocation(3,0);
                            break;
                        case REC:
                            setCursorLocation(6,0);
                            break;
                        case PLAYBACK:
                            setCursorLocation(10,0);
                            break;
                        case OFF:
                            setCursorLocation(13,0);
                    }
                }           

                //Clear up menu settings before leaving
                oldMenu = MENU;
                writeToLCD(CURSOR_OFF, 'c');
                break;

            case FM:
                //Redraw the screen edge only if the menu screen has changed
    			if (oldMenu != menuState)
    			{
                    selectOutput(3);
                    writeToLCD(CLEAR_LCD, 'c');
    				writeToLCD(0x00, 'd'); //Up arrow
    				setCursorLocation(0,1);
    				printLCD((char *)"\x01P-  <<   >>  P+ ");
                    setCursorLocation(5,0);
    				oldMenu = menuState;
                    buttonPressed = 1;
    			}

			    //Volume
                if (buttonState & VOL_UP) volumeUp();
                else if(buttonState & VOL_DOWN) volumeDown();
                
                //Seek
                if ((oldDebouncedState & PRESET_DOWN) && (oldDebouncedState & FREQ_DOWN))
                    seek('d');                
                else if((oldDebouncedState & PRESET_UP) && (oldDebouncedState & FREQ_UP))
                    seek('u');   

                //Preset Changed
				else if (buttonState & PRESET_UP || buttonState & PRESET_DOWN)
				{
                    //Process Preset change
                    if (buttonState & PRESET_UP) preset++;
                    else if (buttonState & PRESET_DOWN) preset--;

                    //Wait incase the second preset button is also pressed
                    __delay_ms(500);
                    buttonState = debounce();
                   
                    //If the other preset button is pressed, save preset
                    if (buttonState & PRESET_UP || buttonState & PRESET_DOWN)
                    {
                        //Undo the preset change
                        if (buttonState & PRESET_UP) preset++;
                        else if (buttonState & PRESET_DOWN) preset--;
                        
                        //Save to EEPROM
                        EEPROMWriteRegister(preset*2,getFrequency()>>8);
                        EEPROMWriteRegister(preset*2+1,getFrequency()&0x00FF);
                        printLCD("S");
                    } else {
                        //Loop preset at ends
                        if (preset<1) preset = 9;
                        if (preset>9) preset = 1;

                        //Tune into preset
                        unsigned int presetFreq = EEPROMReadRegister(preset*2) << 8;
                        presetFreq |= EEPROMReadRegister(preset*2+1);
                        tune(presetFreq);

                        //Display selected preset
                        char presetStr[16];
                        sprintf(presetStr, "P%d:%.1fMHz   ", preset, ((float)getFrequency())/10);
                        setCursorLocation(3,0);
                        printLCD(presetStr);
                        buttonPressed = 0;
                    }
                }

    			//Manual frequency step
    			else if(buttonState & FREQ_UP || buttonState & FREQ_DOWN)
				{
                    if (buttonState & FREQ_UP) tune(getFrequency()+1);
                    if (buttonState & FREQ_DOWN) tune(getFrequency()-1);
    			} 
                
                //Update Frequency on screen
                if (buttonPressed)
                {
                    char freqStr[16];
    				sprintf(freqStr, "  %.1fMHz   ", ((float)getFrequency())/10);
                    setCursorLocation(3,0);
                    printLCD(freqStr);
                }
                break;
                
            case BT:
                if(oldMenu != menuState)
                {
                    //Show bluetooth logo
                    selectOutput(0);
                    writeToLCD(CLEAR_LCD, 'c');
                    printLCD("   Bluetooth  \x02\x03");
                    setCursorLocation(0,1);
                    printLCD("    Receiver  \x04\x05");
                    oldMenu = BT;
                }
                break;
                
            case REC:
                if(oldMenu != menuState)
                {
                    writeToLCD(CLEAR_LCD, 'c');
                    oldMenu = REC;
                    getDateTime(&scheduleSelect);
                }
                
                //Show the dates and time to schedule a recording
                setCursorLocation(0,0);
                char buffer[16];
                sprintf(buffer, "  %02d/%02d %02d:%02d   ", scheduleSelect.date, scheduleSelect.month, scheduleSelect.hour, scheduleSelect.min);
                printLCD(buffer);
                setCursorLocation(0,1);
                printLCD((char *)"\x7f    \x7E    \x2B    \x2D");

                break;

            case PLAYBACK:
                if(oldMenu != menuState)
                {
                    buffer[16];
                    sprintf(buffer, "Recording No.", file);
                    writeToLCD(CLEAR_LCD, 'c');
                    printLCD(buffer);
                    setCursorLocation(0,1);
                    printLCD((char *)"\x7F    \x7E    x    \x07");
                    oldMenu = PLAYBACK;
                }
                                
                //Previous File
                if (buttonState & PRESET_DOWN) file--;
                
                //Next file    
                else if (buttonState & FREQ_DOWN) file ++;
                
                //Exit
                else if (buttonState & FREQ_UP) menuState = MENU;
                
                //Play
                else if(buttonState & PRESET_UP) 
                {
                    menuState = PLAYING; 
                    playRecording(file);
                }
                
                if (file>10) file =1;
                if (file==0) file = 10;
                
                //Play
                if (buttonState & PRESET_UP)
                    continue;
                
                setCursorLocation(14,0);
                char file_Str[4];
                sprintf(file_Str, "%2d", file);
                printLCD(file_Str);
                
                break;
            
            case PLAYING:
                if(oldMenu != menuState)
                {
                    selectOutput(1);
                    buffer[16];
                    sprintf(buffer, "Playing File %d", file);
                    writeToLCD(CLEAR_LCD, 'c');
                    printLCD(buffer);
                    setCursorLocation(0,1);
                    printLCD((char *)"    P    \xFF     ");
                    oldMenu = PLAYING;
                }
                
                //Play/Pause  
                if (buttonState & FREQ_DOWN) pausePlayback();
                
                //Stop
                else if (buttonState & FREQ_UP) 
                {
                    stopPlayback();
                    menuState = PLAYBACK;
                }
                
                break;
                
            case OFF:
                //Turn off audio and LCD backlight
                if(oldMenu != menuState) 
                {
                    selectOutput(5);
                    LATC &= 0b11111011;
                }
                setCursorLocation(0,0);
                struct dateTime currentDateTime;
                getDateTime(&currentDateTime);
                sprintf(buffer, "   %02d/%02d/20%02d   ", currentDateTime.date, currentDateTime.month, currentDateTime.year);
                printLCD(buffer);
                setCursorLocation(0,1);
                sprintf(buffer, "      %02d:%02d     ", currentDateTime.hour, currentDateTime.min);
                printLCD(buffer);
                __delay_ms(500);
        }
    }
}

void interrupt TimerOverflow()
{
    // If timer interrupt
    if(INTCONbits.TMR0IF == 1)
    {
        //Save button states into circular buffer
        State[Index++]=PORTB;
        if(Index>=MAX_CHECKS)Index=0;
        
        //Reset Timer
        INTCONbits.TMR0IF = 0;
        WriteTimer0(0xFC00); //Please use HEX. Decimal don't work
    }
}
