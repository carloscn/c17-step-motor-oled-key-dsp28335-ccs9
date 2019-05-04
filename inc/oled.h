/*
 * oled.h
 *
 *  Created on: 2019年3月10日
 *      Author: multimicro
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#include "iic.h"

#define OLED_CMD  0xAA //写命令
#define OLED_DATA 0xBB //写数据
#define OLED_MODE 0

typedef unsigned char u8;
typedef unsigned long u32;
typedef struct oled_t OLED;

#define SIZE 16
#define XLevelL     0x02
#define XLevelH     0x10
#define Max_Column  128
#define Max_Row     64
#define Brightness  0xFF
#define X_WIDTH     128
#define Y_WIDTH     64


//oled port define
struct oled_hw_t{

    EI2C *i2c;
};

//oled init
struct oled_t {

    struct oled_hw_t hw;
    struct oled_t *self;
    int meme[10];

    void    (*writebyte)( struct oled_t *self, u8 dat, u8 cmd);
    void    (*writedata)(OLED *self,u8 dat);
    void    (*writecommd)(OLED *self,u8 cmd);
    void    (*init_device)( struct oled_t *self);
    void    (*pin_init)( struct oled_t *self);
    void    (*clear)( struct oled_t *self);
    void    (*displayon)( struct oled_t *self);
    void    (*displayoff)( struct oled_t *self);
    void    (*drawpoint)( struct oled_t *self, u8 x, u8 y, u8 t);
    void    (*fill)( struct oled_t *self, u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
    void    (*showchar)( struct oled_t *self, u8 x,u8 y,u8 chr);
    void    (*shownum)( struct oled_t *self, u8 x,u8 y,u32 num,u8 len,u8 size2);
    void    (*showstring)( struct oled_t *self, u8 x,u8 y, u8 *p);
    void    (*setpos)( struct oled_t *self, unsigned char x, unsigned char y);
    void    (*showchinese)( struct oled_t *self, u8 x,u8 y,u8 no);
    void    (*drawbmp)( struct oled_t *self, unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);

};

//OLED控制用函数
void oled_wr_byte(struct oled_t *self, u8 dat, u8 cmd);
void oled_set_pos( struct oled_t *self, unsigned char x, unsigned char y );
void oled_display_on( struct oled_t *self );
void oled_display_off( struct oled_t *self );
void oled_clear( struct oled_t *self );
void oled_showchar( struct oled_t *self, u8 x, u8 y, u8 chr);
u32  oled_pow( struct oled_t *self, u8 m, u8 n );
void oled_shownum( struct oled_t *self, u8 x,u8 y,u32 num,u8 len,u8 size2 );
void oled_showstring( struct oled_t *self, u8 x, u8 y, u8 *chr );
void oled_showchinese( struct oled_t *self, u8 x, u8 y, u8 no);
void oled_drawbmp( struct oled_t *self, unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[] );
void oled_init_device( struct oled_t *self );
void oled_pin_init( struct oled_t *self );
void oled_init( OLED *dev );
void oled_write_data(OLED *dev,u8 dat);
void oled_write_command(OLED *dev,u8 cmd);
extern OLED* oled_new_dev();

#endif /* INC_OLED_H_ */
