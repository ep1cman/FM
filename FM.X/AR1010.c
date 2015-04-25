#include <i2c.h>
#include "AR1010.h"

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

unsigned int AR1010readRegister(unsigned char address)
{
    IdleI2C();
    StartI2C();
    WriteI2C(AR1010_ADDR);
    IdleI2C();
    WriteI2C(address);
    IdleI2C();
    RestartI2C();
    IdleI2C();
    WriteI2C(AR1010_ADDR+1);
    IdleI2C();
    unsigned int val = ReadI2C() << 8;
    AckI2C();
    IdleI2C();
    val |= ReadI2C();
    NotAckI2C();
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

void tune(unsigned int freq)
{
   setBit(TUNE, 0);
   setBit(SEEK, 0);
   unsigned int chan = freq-690;
   writeRegister(2, (ar1010_registers[2]&0xFC00) + chan);
   setBit(TUNE, 1);

   //Wait for STC flag
   while(AR1010readRegister(0x13)&(1<<5) != 1){};
}

const unsigned char VOLUME[19] = {0xF, 0xF, 0xF, 0xF, 0xB, 0xB, 0xB, 0xA, 0x9, 0x8, 0x7, 0x6, 0x6, 0x6, 0x3, 0x3, 0x2, 0x1, 0x0};
const unsigned char VOLUME2[19] = {0x0, 0xC, 0xD, 0xF, 0xC, 0xD, 0xF, 0xF, 0xF, 0xF, 0xF, 0xD, 0xE, 0xF, 0xE, 0xF, 0xF, 0xF, 0xF};
unsigned char currentVolume = 0;

void setVolume(unsigned char volume)
{
    if (volume==255) //Underflow
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
    setBit(HMUTE, 1);
}

void clearHMute()
{
    setBit(HMUTE, 0);
}

void seek(char direction)
{
	setHMute();
	setBit(TUNE, 0);
    setBit(SEEK, 0);

    //Set seek direction
    if (direction == 'u')
    	setBit(SEEKUP, 1);
    if (direction == 'd')
		setBit(SEEKUP, 0);

    setBit(SEEK, 1);

    //Wait for STC flag
	while(AR1010readRegister(0x13)&(1<<5) != 1){};

    clearHMute();

    //Update CHAN with value from READCHAN
    unsigned int chan = AR1010readRegister(0x13) >> 7;
    writeRegister(2, (ar1010_registers[2]&0xFC00) + chan);

}

unsigned int getFrequency()
{
	return (AR1010readRegister(0x13) >> 7) + 690;
}

void powerDown()
{
	setBit(ENABLE, 0);
}

void powerUp()
{
	setBit(ENABLE, 1);

	//Wait for STC flag
	while(AR1010readRegister(0x13)&(1<<5) != 1){};
}
