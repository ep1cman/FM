#include <i2c.h>
#include "MAX4586.h"


//Commands the two MAX4586 mux chips to switch to the desired output (0-3).
void selectOutput(int outputNumber) {
	unsigned char command =  1 << outputNumber;
    
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
}
	
