/*
 * oled.c
 *
 *  Created on: 2019年3月10日
 *      Author: multimicro
 */

#include "oled.h"
#include "oledfont.h"
#include "iic.h"
#include "project.h"



OLED* oled_new_dev()
{

	OLED* dev = (OLED*)malloc(sizeof(OLED)*3);
	dev->hw.i2c = i2c_new_bus();
	dev->clear                 =   &oled_clear;
	dev->displayoff            =   &oled_display_off;
	dev->displayon             =   &oled_display_on;
	dev->pin_init              =   &oled_pin_init;
	dev->setpos                =   &oled_set_pos;
	dev->showchar              =   &oled_showchar;
	dev->showchinese           =   &oled_showchinese;
	dev->shownum               =   &oled_shownum;
	dev->showstring            =   &oled_showstring;
	dev->writebyte             =   &oled_wr_byte;
	dev->writedata             =   &oled_write_data;
	dev->writecommd            =   &oled_write_command;
	dev->init_device           =   &oled_init_device;

	dev->init_device(dev);
	return dev;
}

//oled write command
void oled_write_command(OLED *dev,unsigned char i2c_command) {
	dev->hw.i2c->start(dev->hw.i2c);
	dev->hw.i2c->write(dev->hw.i2c, 0x78 + 0);     //slave address, SA0 = 0;
	dev->hw.i2c->waitack(dev->hw.i2c);
	dev->hw.i2c->write(dev->hw.i2c, 0x00 | 0x00 | 0x00);     //write i2c_command
	dev->hw.i2c->waitack(dev->hw.i2c);
	dev->hw.i2c->write(dev->hw.i2c, i2c_command);
	dev->hw.i2c->waitack(dev->hw.i2c);
	dev->hw.i2c->stop(dev->hw.i2c);
}

//oled write data
void oled_write_data(OLED *dev,unsigned char i2c_data) {
	dev->hw.i2c->start(dev->hw.i2c);
	dev->hw.i2c->write(dev->hw.i2c, 0x78 + 0);     //slave address, SA0 = 0;
	dev->hw.i2c->waitack(dev->hw.i2c);
		//return;

	dev->hw.i2c->write(dev->hw.i2c, 0x00 | 0x40 | 0x00);     //write data
	dev->hw.i2c->waitack(dev->hw.i2c);
	dev->hw.i2c->write(dev->hw.i2c, i2c_data);
	dev->hw.i2c->waitack(dev->hw.i2c);
	dev->hw.i2c->stop(dev->hw.i2c);
}

//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void oled_wr_byte(OLED *dev,u8 dat,u8 cmd)
{

	if (cmd==OLED_DATA) {
		dev->writedata(dev,dat);
	} else {
		dev->writecommd(dev,dat);
	}
}

//coordinate setting
void oled_set_pos(OLED *dev, unsigned char x, unsigned char y) {

	dev->writebyte( dev, 0xb0 + y, OLED_CMD);
	dev->writebyte( dev, ( (x & 0xf0) >> 4) | 0x10, OLED_CMD );
	dev->writebyte( dev, ( (x & 0x0f) | 0x01), OLED_CMD);
}

void oled_display_on( OLED *dev ) {

	dev->writebyte( dev, 0x8d, OLED_CMD );
	dev->writebyte( dev, 0x14, OLED_CMD );
	dev->writebyte( dev, 0xaf, OLED_CMD );
}

void oled_display_off( OLED *dev ) {

	dev->writebyte( dev, 0x8d, OLED_CMD );
	dev->writebyte( dev, 0x10, OLED_CMD );
	dev->writebyte( dev, 0xae, OLED_CMD );
}

void oled_clear( OLED *dev ) {

	u8 i, n;
	for (i = 0; i < 8; ++i) {
		dev->writebyte( dev, 0xb0 + i, OLED_CMD );
		dev->writebyte( dev, 0x00, OLED_CMD );
		dev->writebyte( dev, 0x10, OLED_CMD );
		for (n = 0; n < 128; ++n) {
			dev->writebyte( dev, 0, OLED_DATA);
		}//Update Display
	}
}

void oled_showchar( OLED *dev, u8 x, u8 y, u8 chr) {

	unsigned char c = 0, i = 0;
	c = chr - ' ';//得到偏移后的值
	if ( x > Max_Column - 1 ) { x = 0; y = y + 2; }
	if ( SIZE == 16 ) {
		dev->setpos( dev, x, y );
		for (i = 0; i < 8; ++i) {
			dev->writebyte( dev, F8X16[c*16+i], OLED_DATA );
		}
		dev->setpos( dev, x, y + 1);
		for (i = 0; i < 8; ++i) {
			dev->writebyte( dev, F8X16[c*16+i+8],OLED_DATA );
		}
	} else {
		dev->setpos( dev, x, y+1 );
		for (i = 0; i < 6; ++i) {
			dev->writebyte( dev, F6x8[c][i],OLED_DATA );
		}
	}
}

//m^n函数
u32 oled_pow( OLED *dev, u8 m, u8 n ) {

	u32 result = 1;
	while( n-- ) result *= m;
	return result;
}

//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:模式   0,填充模式;1,叠加模式
//num:数值(0~4294967295);
void oled_shownum( OLED *dev, u8 x,u8 y,u32 num,u8 len,u8 size2 ) {

	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t < len; ++t) {
		temp = ( num/oled_pow( dev, 10, len-t-1 ) ) % 10;
		if ( enshow == 0 && t < ( len - 1 )) {
			if ( temp == 0 ) {
				dev->showchar( dev, x+(size2/2)*t, y, ' ' );
				continue;
			} else {
				enshow = 1;
			}
		}
		dev->showchar( dev, x+(size2/2)*t, y, temp + '0');
	}
}

//显示一个字符号串
void oled_showstring( OLED *dev, u8 x, u8 y, u8 *chr ) {

	unsigned char j = 0;
	while( chr[j] != '\0' ) {
		dev->showchar( dev, x, y, chr[j] );
		x += 8;
		if( x > 120 ) { x = 0; y += 2;  }
		j++;
	}
}

//显示汉字
void oled_showchinese( OLED *dev, u8 x, u8 y, u8 no) {

	u8 t, adder = 0;
	dev->setpos( dev, x, y );
	for (t = 0; t < 16; ++t) {
		dev->writebyte( dev, Hzk[2*no][t],OLED_DATA );
		adder += 1;
	}
	dev->setpos( dev, x, y+1);
	for(t = 0; t < 16; t++) {
		dev->writebyte( dev, Hzk[2*no+1][t],OLED_DATA );
		adder+=1;
	}
}

void oled_pin_init(OLED *dev) {
	// no detail

}

//初始化SSD1306
void oled_init_device(OLED *dev)
{
	DELAY_MS(100);
	dev->writebyte(dev,0xAE,OLED_CMD);//--turn off oled panel
	dev->writebyte(dev,0x00,OLED_CMD);//---set low column address
	dev->writebyte(dev,0x10,OLED_CMD);//---set high column address
	dev->writebyte(dev,0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	dev->writebyte(dev,0x81,OLED_CMD);//--set contrast control register
	dev->writebyte(dev,0xCF,OLED_CMD); // Set SEG Output Current Brightness
	dev->writebyte(dev,0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	dev->writebyte(dev,0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	dev->writebyte(dev,0xA6,OLED_CMD);//--set normal display
	dev->writebyte(dev,0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	dev->writebyte(dev,0x3f,OLED_CMD);//--1/64 duty
	dev->writebyte(dev,0xD3,OLED_CMD);//-set display offset   Shift Mapping RAM Counter (0x00~0x3F)
	dev->writebyte(dev,0x00,OLED_CMD);//-not offset
	dev->writebyte(dev,0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	dev->writebyte(dev,0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	dev->writebyte(dev,0xD9,OLED_CMD);//--set pre-charge period
	dev->writebyte(dev,0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	dev->writebyte(dev,0xDA,OLED_CMD);//--set com pins hardware configuration
	dev->writebyte(dev,0x12,OLED_CMD);
	dev->writebyte(dev,0xDB,OLED_CMD);//--set vcomh
	dev->writebyte(dev,0x40,OLED_CMD);//Set VCOM Deselect Level
	dev->writebyte(dev,0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	dev->writebyte(dev,0x02,OLED_CMD);//
	dev->writebyte(dev,0x8D,OLED_CMD);//--set Charge Pump enable/disable
	dev->writebyte(dev,0x14,OLED_CMD);//--set(0x10) disable
	dev->writebyte(dev,0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	dev->writebyte(dev,0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)
	dev->writebyte(dev,0xAF,OLED_CMD);//--turn on oled panel
	dev->clear(dev);

	dev->writebyte(dev,0xAF,OLED_CMD); /*display ON*/
	dev->clear(dev);
	dev->setpos(dev,0,0);
}

void oled_init( OLED *dev ) {


}
