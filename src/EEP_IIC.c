// TMS320F2833x   Project.
/****************************************************************************/
/*                                                                          */
/*  @file       : *_IIC.c                                                   */
/*  @Copyright  : DelvisBeans.(c) 2015 MULTIBEANS_ORG All rights reserved.  */
/*  @Revision   : Ver 1.0.                                                  */
/*  @Data       : 2015.04.15 Realse.                                        */
/*  @Belong     : PROJECT.                                                  */
/*                                                                          */
/*  **code : (GBK/GB2312) in Windows 7_x64. CCS v6.0 platform.              */
/****************************************************************************/
/*  @Attention:                                                             */
/*  ---------------------------------------------------------------------   */
/*  |    Data    |  Behavior |     Offer     |          Content         |   */
/*  | 2015.04.15 |   create  |DelvisBeans(M) | ceate the document.      |   */
/*  ---------------------------------------------------------------------   */
/*                                                            MULTIBEANS.   */
/****************************************************************************/
#include "DSP2833x_Device.h"

#define                          SDA                                 GpioDataRegs.GPBDAT.bit.GPIO49         // SDA
#define                          SCL                                 GpioDataRegs.GPBDAT.bit.GPIO48         // SCL

#define                          SDA_IN                              EALLOW;GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;EDIS
#define                          SDA_OUT                             EALLOW;GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;EDIS
#define                          SCL_IN                              EALLOW;GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;EDIS
#define                          SCL_OUT                             EALLOW;GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;EDIS
#define                          IIC_START_DELAY                     200
#define                          IIC_NORMAL_DELAY                    200
#define                          IIC_LONG_DELAY                      2000
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////                   º¯ÊýÁÐ±í                            ////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Uint16 API_UINT16_I2cByteRead( Uint16 deviceAddr, Uint16 regAddr );
Uint16 API_UINT16_I2cByteWrite( Uint16 deviceAddr, Uint16 regAddr, unsigned char data);
Uint16 BL_UINT16_I2cSlaveAck( void );
Uint16 BL_UINT16_I2cReceive( void );
void BL_VOID_I2cTransmit( Uint16 _uint16_data );
void BL_VOID_I2cSendStop( void );
void BL_VOID_I2cSendStart( void  );
void BL_VOID_I2cSendNack( void );
void BL_VOID_I2cSendAck( void );
//////////////////////////////////////////////////////////////////////////////
void BL_VOID_I2cSendStart( void  ) {

    SDA_OUT;
    SCL_OUT;

    SCL = 1;
    delay( IIC_START_DELAY );
    SDA = 1;
    delay( IIC_START_DELAY );
    SDA = 0;
    delay( IIC_START_DELAY );
    SCL = 0;
}
//////////////////////////////////////////////////////////////////////////////
void BL_VOID_I2cSendStop( void ) {

    SDA_OUT;
    SCL_OUT;


    SDA = 0;
    delay( IIC_NORMAL_DELAY );
    SCL = 1;
    delay( IIC_NORMAL_DELAY );
    SDA = 1;
    delay( IIC_NORMAL_DELAY );

}
//////////////////////////////////////////////////////////////////////////////
void BL_VOID_I2cTransmit( Uint16 _uint16_data ) {

    Uint16 i;

    SDA_OUT;
    SCL_OUT;
    SCL = 0;
    delay( IIC_NORMAL_DELAY );

    for( i = 8; i != 0 ; i-- ) {
        if ( (_uint16_data & 0x80) ) {
            SDA = 1;
        }else {
            SDA = 0;
        }
        delay( IIC_NORMAL_DELAY );
        SCL = 1;
        delay( IIC_NORMAL_DELAY );
        SCL = 0;
        delay( IIC_NORMAL_DELAY );
        _uint16_data = _uint16_data << 1;
    }

}
//////////////////////////////////////////////////////////////////////////////
Uint16 BL_UINT16_I2cReceive( void ) {

    Uint16 i;
    Uint16 output;

    SDA_OUT;
    SDA_OUT;

    SDA = 1;

    SDA_IN;
    delay( IIC_NORMAL_DELAY );
    for( i = 8; i != 0; i --) {
        SCL = 0;
        delay( IIC_NORMAL_DELAY );
        SCL = 1;
        output = output << 1;
        if( SDA ) {
            output |= 1;
        }

        delay( IIC_NORMAL_DELAY );
        delay( IIC_NORMAL_DELAY );
    }
    SCL = 0;

    return output;
}
//////////////////////////////////////////////////////////////////////////////
Uint16 BL_UINT16_I2cSlaveAck( void ) {

    Uint16 ack;
    delay( IIC_NORMAL_DELAY );

    SCL_OUT;
    SDA_IN;

    SCL = 1;
    delay( IIC_LONG_DELAY );
    if( SDA == 1 ) {
        ack = 1;
    }else {
        ack = 0;
    }
    delay( IIC_NORMAL_DELAY );

    SDA_OUT;
    SCL = 0;
    delay( IIC_NORMAL_DELAY );
    return ack;

}
//////////////////////////////////////////////////////////////////////////////
void BL_VOID_I2cSendAck( void ) {
    SDA_OUT;
    SDA_OUT;
    delay( IIC_NORMAL_DELAY );
    SCL = 0;
    SDA = 0;
    delay( IIC_NORMAL_DELAY );
    SCL = 1;
    delay( IIC_NORMAL_DELAY );
    SCL = 0;
    delay( IIC_NORMAL_DELAY );
    SDA = 1;

}
//////////////////////////////////////////////////////////////////////////////
void BL_VOID_I2cSendNack( void ) {

    SDA_OUT;
    SDA_OUT;
    SDA = 1;
    delay( IIC_NORMAL_DELAY );
    SCL = 1;
    delay( IIC_NORMAL_DELAY );
    SCL = 0;

}
//////////////////////////////////////////////////////////////////////////////
Uint16 API_UINT16_I2cByteWrite( Uint16 deviceAddr, Uint16 regAddr, unsigned char  data) {

    Uint16 ack;

    BL_VOID_I2cSendStart();
    delay( IIC_NORMAL_DELAY );
    BL_VOID_I2cTransmit( deviceAddr );
    delay( IIC_LONG_DELAY );

    ack = BL_UINT16_I2cSlaveAck();

    if( ack == 0 ) {
        BL_VOID_I2cTransmit( regAddr >> 8 );
    }
    if( ack == 0 ) {
        ack = BL_UINT16_I2cSlaveAck();
    }
    if( ack == 0 ) {
        BL_VOID_I2cTransmit( regAddr & 0xFF );
    }
    if( ack == 0 ) {
        ack = BL_UINT16_I2cSlaveAck();
    }
    if( ack == 0 ) {
        BL_VOID_I2cTransmit( data );
    }
    if( ack == 0 ) {
        ack = BL_UINT16_I2cSlaveAck();
    }

    BL_VOID_I2cSendStop();


    return (ack);

}
//////////////////////////////////////////////////////////////////////////////
Uint16 API_UINT16_I2cByteRead( Uint16 deviceAddr, Uint16 regAddr ) {

    Uint16 ack, data;

    BL_VOID_I2cSendStart();
    BL_VOID_I2cTransmit( deviceAddr );
    ack = BL_UINT16_I2cSlaveAck();

    if( ack == 0 ) {
        BL_VOID_I2cTransmit( regAddr >> 8 );
    }
    if( ack == 0 ) {
        ack = BL_UINT16_I2cSlaveAck();
    }
    if( ack == 0 ) {
        BL_VOID_I2cTransmit( regAddr & 0xFF );
    }
    if( ack == 0 ) {
        ack = BL_UINT16_I2cSlaveAck();
    }
    if( ack == 0 ) {
        BL_VOID_I2cSendStop();
    }

    BL_VOID_I2cSendStart();
    BL_VOID_I2cTransmit( deviceAddr | 0x01 );
    ack = BL_UINT16_I2cSlaveAck();
    if( ack == 0 ) {
        data = BL_UINT16_I2cReceive();
    }
    if( ack == 0 ) {
        ack = BL_UINT16_I2cSlaveAck();
    }

    BL_VOID_I2cSendStop();


    return (data);

}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
////////////////                END OF FILE.              ////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
