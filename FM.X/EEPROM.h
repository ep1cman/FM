/*
 * File:   EEPROM.h
 * Author: Phil
 *
 * Created on 25 April 2015, 20:30
 */

#ifndef EEPROM_H
#define	EEPROM_H

#define EEPROM_ADDR  0xAE


unsigned char EEPROMReadRegister(unsigned char address);
unsigned char EEPROMWriteRegister(unsigned char address, unsigned char data);

#endif	/* EEPROM_H */

