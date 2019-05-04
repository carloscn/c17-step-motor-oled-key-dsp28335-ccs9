/*
 * i2c.c
 *
 *  Created on: 2019Äê3ÔÂ10ÈÕ
 *      Author: multimicro
 */
#include "project.h"
#include "iic.h"
#include <stdlib.h>

#define			I2C_DELAY()			DELAY_US(12)
#define			MEM_DELAY()			asm("  nop")


//i2c start
void i2c_start(EI2C *self){
#if bl
	SDA_OUT();

    SDA_HIGH();
    SCK_HIGH();

    I2C_DELAY();
    SDA_LOW();
    I2C_DELAY();
    SCK_LOW();
    I2C_DELAY();
#endif
    BL_VOID_I2cSendStart();
}

//i2c stop
void i2c_stop(EI2C *self) {
#if bl
	SDA_OUT();
	SCK_HIGH();
    SDA_LOW();
    I2C_DELAY();
    SDA_HIGH();
    I2C_DELAY();
#endif
    BL_VOID_I2cSendStop();
}

//i2c write data
void i2c_write_byte(EI2C *self, unsigned char i2c_byte) {
#if bl
    unsigned char i;
    SDA_OUT();
    for (i = 0; i < 8; i++) {
    	SCK_LOW();
        if (i2c_byte & 0x80) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        SCK_HIGH();
        I2C_DELAY();
        SCK_LOW();
        i2c_byte<<=1;
    }
    SDA_HIGH();
    SCK_HIGH();
    I2C_DELAY();
#endif
    BL_VOID_I2cTransmit(i2c_byte);
}

//i2c write command
void i2c_write_command(EI2C *dev,unsigned char i2c_command) {
    dev->start(dev);
    dev->write(dev, 0x78);     //slave address, SA0 = 0;
    dev->waitack(dev);
    dev->write(dev, 0x00);     //write data
    dev->waitack(dev);
    dev->write(dev, i2c_command);
    dev->waitack(dev);
    dev->stop(dev);
}

//i2c write data
void i2c_write_data(EI2C *dev,unsigned char i2c_data) {
    dev->start(dev);
    dev->write(dev, 0x78);     //slave address, SA0 = 0;
    dev->waitack(dev);
    dev->write(dev, 0x40);     //write data
    dev->waitack(dev);
    dev->write(dev, i2c_data);
    dev->waitack(dev);
    dev->stop(dev);
}


//i2c wait ack;
void i2c_wait_ack(EI2C *self) {

	uint8_t count = 0;
    SCK_HIGH();
    I2C_DELAY();
    SCK_LOW();
    SDA_IN();

    SDA_OUT();

}

uint8_t i2c_wait_to_ack(EI2C* self)
{
    uint8_t redata;
    uint8_t count = 0;
    uint8_t t = 0;

    SDA_IN();
    SCK_HIGH();
    I2C_DELAY();
    do{
    	t = SDA_READ();
    	count ++;
    	DELAY_US(1);
    	if (count > 255) {
        	//i2c_stop(self);
            redata = 1;
            count = 0;
            break;
    	}
    }while(t == 1);

    SDA_OUT();
	SCK_LOW();
	I2C_DELAY();
    redata = 0;

    return redata;
}

void i2c_init(EI2C *self) {



}
EI2C* i2c_new_bus()
{
	EI2C* bus = (EI2C*)malloc(sizeof(EI2C)*3);

	bus->start         =   &i2c_start;
	bus->stop          =   &i2c_stop;
	bus->write         =   &i2c_write_byte;
	bus->waitack       =   &i2c_wait_ack;
	bus->wait_to_ack  	= &i2c_wait_to_ack;

    EALLOW;
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
	GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
	GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;
	GpioDataRegs.GPBDAT.bit.GPIO49 = 1;
	GpioDataRegs.GPBDAT.bit.GPIO48 = 1;
	GpioCtrlRegs.GPBPUD.bit.GPIO49 = 1;
	GpioCtrlRegs.GPBPUD.bit.GPIO48 = 1;

	EDIS;
	return bus;
}
