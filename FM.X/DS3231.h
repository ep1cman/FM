/*
 * File:   DS3231.h
 * Author: Sebastian Goscik
 *
 * Created on 23 March 2015, 19:44
 */

#ifndef DS3231_H
#define	DS3231_H

#define DS3231_ADDR  0xD0

#define SECONDS 0x00
#define MINUTES 0x01
#define HOURS 0x02
#define DAY 0x03
#define DATE 0x04
#define MONTH 0x05
#define YEAR 0x06

struct dateTime
{
    unsigned char sec:6;
    unsigned char min:6;
    unsigned char hour:5;
    unsigned char day:3;
    unsigned char date:5;
    unsigned char month:4;
    unsigned char year:7;
};

unsigned char DS3231ReadRegister(unsigned char address);
unsigned char BCD2Dec(unsigned char value);

void getDateTime(struct dateTime* currentDateTime);


#endif	/* DS3231_H */

