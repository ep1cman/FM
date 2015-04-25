unsigned char EEPROMReadRegister(unsigned char address) {
	IdleI2C();
	StartI2C();
	WriteI2C(EEPROM_ADDR);
	IdleI2C();
	WriteI2C(0x00);
	IdleI2C();	
	WriteI2C(address);
	IdleI2C();
	RestartI2C();
	IdleI2C();
	WriteI2C(EEPROM_ADDR+1);
	IdleI2C();
	unsigned char val = ReadI2C();
	IdleI2C();
	NotAckI2C();
	StopI2C();
	return val;
}

void EEPROMWriteRegister(unsigned char address, unsigned char data) {
	IdleI2C();
	StartI2C();
	WriteI2C(EEPROM_ADDR);
	IdleI2C();
	WriteI2C(0x00);
	IdleI2C();
	WriteI2C(address);
	IdleI2C();
	WriteI2C(data);
	IdleI2C();
	StopI2C();
}
