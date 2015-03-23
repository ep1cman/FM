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
#include <i2c.h>
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config OSC = INTIO67       // Oscillator (External RC oscillator, port function on RA6)
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

#define AR1010_ADDR 0x20

#define TUNE 9
#define SEEK 14

#define _XTAL_FREQ 8000000

unsigned int ar1010_registers[18] = {
  0xFFFB, // R0:  1111 1111 1111 1011
  0x5B15, // R1:  0101 1011 0001 0101 - Mono (D3), Softmute (D2), Hardmute (D1)
  0xD0B9, // R2:  1101 0000 1011 1001 - Tune/Channel
  0xA010, // R3:  1010 0000 0001 0000 - Seekup (D15), Seek bit (D14), Space 100kHz (D13), Seek threshold: 16 (D6-D0)
  0x0780, // R4:  0000 0111 1000 0000
  0x28AB, // R5:  0010 1000 1010 1011
  0x6400, // R6:  0110 0100 0000 0000
  0x1EE7, // R7:  0001 1110 1110 0111
  0x7141, // R8:  0111 0001 0100 0001
  0x007D, // R9:  0000 0000 0111 1101
  0x82C6, // R10: 1000 0010 1100 0110 - Seek wrap (D3)
  0x4E55, // R11: 0100 1110 0101 0101
  0x970C, // R12: 1001 0111 0000 1100
  0xB845, // R13: 1011 1000 0100 0101
  0xFC2D, // R14: 1111 1100 0010 1101 - Volume control 2 (D12-D15)
  0x8097, // R15: 1000 0000 1001 0111
  0x04A1, // R16: 0000 0100 1010 0001
  0xDF61  // R17: 1101 1111 0110 0001
};

void writeRegister(unsigned char address, unsigned int data)
{
    IdleI2C();               //Wait for I2C bus to be free
    StartI2C();              //Place a START condition on the bus
    IdleI2C();               //Wait for START condition to end
    WriteI2C(AR1010_ADDR);   //Write AR1010 address to the bus
    IdleI2C();               //Wait for ACK
    WriteI2C(address);       //Tell the AR1010 what register we are writing to
    IdleI2C();               //Wait for ACK
    WriteI2C(data >> 8);     //Write upper byte of register
    IdleI2C();               //Wait for ACK
    WriteI2C(data & 0x00FF); //Write lower byte of register
    IdleI2C();               //Wait for ACK
    StopI2C();               //Place stop condition on the bus
    ar1010_registers[address] = data; //Save data written to array
}

unsigned int readRegister(unsigned char address)
{
    IdleI2C();
    StartI2C();
    WriteI2C(AR1010_ADDR);
    IdleI2C();
    WriteI2C(address);
    IdleI2C();
    StartI2C();
    WriteI2C(AR1010_ADDR+1);
    IdleI2C();

    unsigned int val = ReadI2C() << 8;
    IdleI2C();
    val &= ReadI2C();
    IdleI2C();
    StopI2C();
    if (address<18)
    {
        ar1010_registers[address] = val;
    }
    return val;
}

void setBit(unsigned char address, unsigned char bitToSet, unsigned char set)
{
    unsigned int val = ar1010_registers[address];
    if (set)
    {
        val |= (1<<bitToSet);
    } else {
        val &= ~(1<<bitToSet);
    }
    writeRegister(address, val);
}

void initAR1010()
{
    IdleI2C();
    StartI2C();
    IdleI2C();
    WriteI2C(AR1010_ADDR);   //Write AR1010 address to the bus
    IdleI2C();               //Wait for ACK
    WriteI2C(0x01);       //Tell the AR1010 what register we are writing to
    IdleI2C();            
    
    for (int i=1; i<18; i++)
    {
        
        WriteI2C(ar1010_registers[i] >> 8);     //Write upper byte of register
        IdleI2C();               //Wait for ACK
        WriteI2C(ar1010_registers[i] & 0x00FF); //Write lower byte of register
        IdleI2C();               //Wait for ACK
    }
    
    StopI2C();
    IdleI2C();
    writeRegister(0, ar1010_registers[0]);
}

void tune(int freq)
{
   setBit(0x02, TUNE, 0);
   setBit(0x03, SEEK, 0);
   unsigned int chan = freq-690;
   writeRegister(2, (ar1010_registers[2]&0xFC00) + chan);
   setBit(0x02, TUNE, 1);

   //Wait for STC flag
   while(readRegister(0x13)&(1<<5) != 1){};
}

const unsigned char VOLUME[19] = {0xF, 0xF, 0xF, 0xF, 0xB, 0xB, 0xB, 0xA, 0x9, 0x8, 0x7, 0x6, 0x6, 0x6, 0x3, 0x3, 0x2, 0x1, 0x0};
const unsigned char VOLUME2[19] = {0x0, 0xC, 0xD, 0xF, 0xC, 0xD, 0xF, 0xF, 0xF, 0xF, 0xF, 0xD, 0xE, 0xF, 0xE, 0xF, 0xF, 0xF, 0xF};

unsigned char currentVolume = 0;

void setVolume(char volume)
{
    if (volume<0 ) 
        volume = 0;
    if (volume>18) 
        volume = 18;
    unsigned int vol = (ar1010_registers[0x03]&0xF87F) | (VOLUME[volume]<<7);
    unsigned int vol2 = (ar1010_registers[0x0E]&0x0FFF) | (VOLUME2[volume]<<12);
    writeRegister(0x03, vol);
    writeRegister(0x0E, vol2);
    currentVolume = volume;
}

void volumeUp()
{
    setVolume(++currentVolume);
}

void volumeDown()
{
    setVolume(--currentVolume);
}

void setHMute()
{
    setBit(1, 1, 1);
}

void clearHMute()
{
    setBit(1,1,0);
}

int main(int argc, char** argv) {
    OpenI2C(MASTER, SLEW_OFF);
    TRISC = 0b00011000;
    initAR1010();
    for(int i=0; i<3000; i++){};
    tune(964);
    while(1);
    return (EXIT_SUCCESS);
}
