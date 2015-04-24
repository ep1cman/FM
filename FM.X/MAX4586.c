#include <i2c.h>
#include "MAX4586.h"


//Commands the two MAX4586 mux chips to switch to the desired output (1-4). Returns 0 on success, 1 on error.
int selectOutput(int outputNumber) {
	unsigned char command = 0x00;

	// Determine the correct command to send.
		switch(outputNumber) {
			case 1:
			command = OUTPUT_1;
			case 2:
			command = OUTPUT_2;	
			case 3:
			command = OUTPUT_3;
			case 4:
			command = OUTPUT_4;
			default:
			return 1;
		}		

	//Write command to the left mux.
	IdleI2C();
	StartI2C();
	IdleI2C();
	WriteI2C(MAX4586_L_ADDR);
	IdleI2C();
	WriteI2C(command);
	IdleI2C();
	StopI2C();

	//Write command to the right mux.
	IdleI2C();
	StartI2C();
	WriteI2C(MAX4586_R_ADDR);
	IdleI2C();
	WriteI2C(command);
	IdleI2C();
	StopI2C();
	
	return 0;
}
	
