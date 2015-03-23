/*
 * File:   DS3231.c
 * Author: Sebastian Goscik
 *
 * Created on 23 March 2015, 19:57
 */

#include <i2c.h>
#include "DS3231.h"

unsigned char BCD2Dec(unsigned char value)
{
    return ((value & 0x0F) + (value & 0xF0)*10);
}

unsigned char DS3231ReadRegister(unsigned char address)
{
    IdleI2C();
    StartI2C();
    WriteI2C(DS3231_ADDR);
    IdleI2C();
    WriteI2C(address);
    IdleI2C();
    StartI2C();
    WriteI2C(DS3231_ADDR+1);
    IdleI2C();

    unsigned int val = ReadI2C();
    IdleI2C();
    StopI2C();
    return val;
}

//Only works for 12 hour clock at the moment
void getDateTime(struct dateTime* currentDateTime)
{
    currentDateTime->sec = BCD2Dec(DS3231ReadRegister(SECONDS));
    currentDateTime->min = BCD2Dec(DS3231ReadRegister(MINUTES));
    //Masks out unwanted 24h bits
    currentDateTime->min = BCD2Dec(DS3231ReadRegister(MINUTES) & 0x3F);

    currentDateTime->day = BCD2Dec(DS3231ReadRegister(DAY));
    currentDateTime->date = BCD2Dec(DS3231ReadRegister(DATE));
    currentDateTime->month = BCD2Dec(DS3231ReadRegister(MONTH));
    currentDateTime->year = BCD2Dec(DS3231ReadRegister(YEAR));
}
