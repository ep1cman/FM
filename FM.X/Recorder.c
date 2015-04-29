/*
 * File:   RECORDER.h
 * Author: Phil
 *
 * Created on 27 April 2015, 02:26
 */

#include <i2c.h>
#include "Recorder.h"

#define _XTAL_FREQ 1000000

void startRecording(unsigned char fileNumber) {
	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR);
	IdleI2C();
	WriteI2C(0x01);
	IdleI2C();
	WriteI2C(fileNumber);
	IdleI2C();
	StopI2C();

	__delay_ms(500);

	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR+1);
	IdleI2C();
	unsigned char rtn = ReadI2C();
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
}

void stopRecording() {
	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR);
	IdleI2C();
	WriteI2C(0x02);
	IdleI2C();
	StopI2C();

	__delay_ms(500);

	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR+1);
	IdleI2C();
	unsigned char rtn = ReadI2C();
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
}

void playRecording(unsigned char fileNumber) {
	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR);
	IdleI2C();
	WriteI2C(0x04);
	IdleI2C();
	WriteI2C(fileNumber);
	IdleI2C();
	StopI2C();

	__delay_ms(500);

	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR+1);
	IdleI2C();
	unsigned char rtn = ReadI2C();
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
}

void pausePlayback() {
	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR);
	IdleI2C();
	WriteI2C(0x05);
	IdleI2C();
	StopI2C();

	__delay_ms(500);

	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR+1);
	IdleI2C();
	unsigned char rtn = ReadI2C();
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
}

void stopPlayback() {
	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR);
	IdleI2C();
	WriteI2C(0x06);
	IdleI2C();
	StopI2C();

	__delay_ms(500);

	IdleI2C();
	StartI2C();
	WriteI2C(RECORDER_ADDR+1);
	IdleI2C();
	unsigned char rtn = ReadI2C();
	IdleI2C();
	NotAckI2C();
	IdleI2C();
	StopI2C();
}
