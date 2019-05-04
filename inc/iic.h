/*
 * iic.h
 *
 *  Created on: 2019Äê3ÔÂ10ÈÕ
 *      Author: multimicro
 */

#ifndef INC_IIC_H_
#define INC_IIC_H_

#include <DSP2833x_Device.h>

typedef struct i2c_t EI2C;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

#define         I2C_SCK_DIR                 (P3DIR)
#define         I2C_SCK_OUT                 (P3OUT)
#define         I2C_SCK_PIN_NUM             (BIT3)
#define         I2C_SDA_DIR                 (P3DIR)
#define         I2C_SDA_OUT                 (P3OUT)
#define         I2C_SDA_PIN_NUM             (BIT2)

#define         SDA_IN()      EALLOW; GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0; EDIS
#define         SDA_OUT()      EALLOW; GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1; EDIS

#define			SDA_READ()		(GpioDataRegs.GPBDAT.bit.GPIO49)

#define         SCK_HIGH()          GpioDataRegs.GPBDAT.bit.GPIO48 = 1
#define         SCK_LOW()           GpioDataRegs.GPBDAT.bit.GPIO48 = 0
#define         SDA_HIGH()          GpioDataRegs.GPBDAT.bit.GPIO49 = 1
#define         SDA_LOW()           GpioDataRegs.GPBDAT.bit.GPIO49 = 0

enum ack_flag_t {
    with_ack,
    no_ack
};

enum i2c_flags_t {
    ACK_ERROR,
    ACK_OK
};

//I2C device type
enum i2c_type_t {
    I2C_MCU,
    I2C_EEPROM
};

//I2C hardware port
struct i2c_hw_t {
    int sck_pin_row_dir;
    int sck_pin_row_out;
    int sck_pin_bit;
    int sda_pin_row_dir;
    int sda_pin_row_out;
    int sda_pin_bit;
};

//I2C device description
struct i2c_t {
    enum i2c_type_t type;
    struct i2c_hw_t hw;
    struct i2c_t *self;
    long delay;
    short chip_no_ack;

    long meme[10];

    void (*init)(struct i2c_t *self);
    void (*start)(struct i2c_t *self);
    void (*stop)(struct i2c_t *self);
    void (*write)(struct i2c_t *self,unsigned char i2c_byte);
    void (*send)(struct i2c_t *self,uint8_t data);
    int (*get)(struct i2c_t *self);
    void (*waitack)(struct i2c_t *self);
    uint8_t (*wait_to_ack)(EI2C* self);
//    enum i2c_flags_t  (*waitack)(struct i2c_t *self);
//    void (*setack)(struct i2c_t *self,enum ack_flag_t ack);
//
//    enum i2c_flags_t (*write_byte)(struct i2c_t *self,uint8_t dev_addr,uint8_t addr,uint8_t data);
//    enum i2c_flags_t (*read_byte)(struct i2c_t *self,uint8_t dev_addr,uint8_t addr,uint8_t *data);
//    enum i2c_flags_t (*read_buf)(struct i2c_t *self,uint8_t *data,uint16_t length,uint8_t addr,uint8_t device_addr);

};


void i2c_init(struct i2c_t *self);
void i2c_delay(struct i2c_t *self);
void i2c_start(struct i2c_t *self);
void i2c_stop(struct i2c_t *self);
//void set_sda_input(struct i2c_t *self);
//void set_sda_out(struct i2c_t *self);
//void i2c_send(struct i2c_t *self,uint8_t data);
//int i2c_read_data(struct i2c_t *self);
//enum i2c_flags_t i2c_write(struct i2c_t *self,uint8_t dev_addr,uint8_t addr,uint8_t data);
//enum i2c_flags_t i2c_read(struct i2c_t *self,uint8_t dev_addr,uint8_t addr,uint8_t *data);
//enum i2c_flags_t i2c_read_buffer(struct i2c_t *self,uint8_t *data,unsigned int length,uint8_t addr,uint8_t device_addr);

void i2c_write_byte(struct i2c_t *self, unsigned char i2c_byte);
void i2c_write_command(struct i2c_t *self,unsigned char i2c_command);
void i2c_write_data(struct i2c_t *self,unsigned char i2c_data);
void i2c_wait_ack(struct i2c_t *self);
extern EI2C* i2c_new_bus();





//no more





#endif /* INC_IIC_H_ */
