//*****************************************************************************
// AudioProc programm 
// Doynikov Andrew 
// 11.04.2008 ver: 2.0 
//*****************************************************************************
// I2C FUNCTIONS
//*****************************************************************************

#include <avr/io.h>
#include "i2c.h"
// ============================================================================
void delay(unsigned long us)
{
	us = us * 2;
	while (us) {
		asm volatile ("nop\n\t"::);
		asm volatile ("nop\n\t"::);
		us--;
	}
}

// ============================================================================
void i2c_init(void)
{
	I2C_SDA_OUT_REG &= ~(_BV(SDA_PIN));
	I2C_SCL_OUT_REG &= ~(_BV(SCL_PIN));
	RELEASE_I2C_BUS();
	delay(I2C_TIMEOUT);
	i2c_start();
	delay(I2C_TIMEOUT);
	i2c_stop();
	delay(I2C_TIMEOUT);
	return;
}

// ============================================================================
void i2c_start(void)
{
	RELEASE_I2C_BUS();
	delay(I2C_DELAY);
	SDA_0();
	delay(I2C_DELAY);
	SCL_0();
	delay(I2C_DELAY);
	return;
}

// ============================================================================
void i2c_stop(void)
{
	SDA_0();
	SCL_1();
	delay(I2C_DELAY);
	SDA_1();
	delay(I2C_DELAY);
	SCL_0();
	delay(I2C_DELAY);
	return;
}

// ============================================================================
unsigned char i2c_write(unsigned char data)
{
	register unsigned char bit = 0;
	for (bit = 0; bit <= 7; bit++) {
		if (data & 0x80) {
			SDA_1();
		} else {
			SDA_0();
		}
		SCL_1();
		delay(I2C_DELAY);
		SCL_0();
		delay(I2C_DELAY);
		data = (data << 1);
	}
	RELEASE_I2C_BUS();
	delay(I2C_DELAY);
	if (bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN)) {
		SCL_0();
		delay(I2C_DELAY);
	} else {
		delay(I2C_TIMEOUT);
		if (bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN)) {
			SCL_0();
			delay(I2C_DELAY);
		} else {
			return (I2C_ERROR_DEVICE_NOT_RESPONDING);
		}
	}
	if (bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN)) {
		delay(I2C_TIMEOUT);
		if (bit_is_clear(I2C_SDA_PIN_REG, SDA_PIN)) {
			return (I2C_ERROR_DEVICE_BUSY);
		}
	}
	return (I2C_NO_ERROR);
}

// ============================================================================
unsigned char i2c_read(unsigned char ack)
{
	register unsigned char bit = 0, data = 0;
	SDA_1();
	for (bit = 0; bit <= 7; bit++) {
		SCL_1();
		delay(I2C_DELAY);
		data = (data << 1);
		if (bit_is_set(I2C_SDA_PIN_REG, SDA_PIN)) {
			data++;
		}
		SCL_0();
		delay(I2C_DELAY);
	}
	if (ack == 1) {
		SDA_0();
	} else {
		SDA_1();
	}
	SCL_1();
	delay(I2C_DELAY);
	SCL_0();
	delay(I2C_DELAY);
	return data;
}

// ============================================================================
