/* 
 * File:   AR1010.h
 * Author: Sebastian
 *
 * Created on 23 March 2015, 01:09
 */

#ifndef AR1010_H
#define	AR1010_H

#define AR1010_ADDR 0x20

#define ENABLE 0x00, 0
#define HMUTE 0x01, 1
#define SEEKUP 0x02, 15
#define TUNE 0x02, 9
#define SEEK 0x03, 14

// Utility functions
void writeRegister(unsigned char address, unsigned int data);
unsigned int readRegister(unsigned char address);
void setBit(unsigned char address, unsigned char bitToSet, unsigned char set);

void initAR1010();

// Frequency functions
void tune(unsigned int freq);
void seek(char direction);
unsigned int getFrequency();

// Volume functions
void setVolume(char volume);
void volumeUp();
void volumeDown();
void setHMute();
void clearHMute();

// Power functions
void powerDown();
void powerUp();

#endif	/* AR1010_H */

