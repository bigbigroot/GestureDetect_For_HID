
/**
 * @file vl6180_i2c.c
 *
 * Copyright (C) 2019 ST MicroElectronics
 *
 * provide variable word size byte/Word/dword VL6180 register access via i2c
 *
 */
#include "vl6180_i2c.h"

#ifndef I2C_BUFFER_CONFIG
#error "I2C_BUFFER_CONFIG not defined"
/* TODO you must define value for  I2C_BUFFER_CONFIG in configuration or platform h */
#endif


#if I2C_BUFFER_CONFIG == 0
    /* GLOBAL config buffer */
    uint8_t i2c_global_buffer[VL6180_MAX_I2C_XFER_SIZE];

    #define DECL_I2C_BUFFER
    #define VL6180_GetI2cBuffer(dev, n_byte)  i2c_global_buffer

#elif I2C_BUFFER_CONFIG == 1
    /* ON STACK */
    #define DECL_I2C_BUFFER  uint8_t LocBuffer[VL6180_MAX_I2C_XFER_SIZE];
    #define VL6180_GetI2cBuffer(dev, n_byte)  LocBuffer
#elif I2C_BUFFER_CONFIG == 2
    /* user define buffer type declare DECL_I2C_BUFFER  as access  via VL6180_GetI2cBuffer */
    #define DECL_I2C_BUFFER
#else
#error "invalid I2C_BUFFER_CONFIG "
#endif


int VL6180_WrByte(VL6180Dev_t dev, uint16_t index, uint8_t data){
    int  status;
    uint8_t *buffer;
    DECL_I2C_BUFFER
    VL6180_I2C_USER_VAR

    VL6180_GetI2CAccess(dev);

    buffer=VL6180_GetI2cBuffer(dev,3);
    buffer[0]=index>>8;
    buffer[1]=index&0xFF;
    buffer[2]=data;

    status=VL6180_I2CWrite(dev, buffer,(uint8_t)3);
    VL6180_DoneI2CAcces(dev);
    return status;
}

int VL6180_WrWord(VL6180Dev_t dev, uint16_t index, uint16_t data){
    int  status;
    DECL_I2C_BUFFER
    uint8_t *buffer;
    VL6180_I2C_USER_VAR

    VL6180_GetI2CAccess(dev);

    buffer=VL6180_GetI2cBuffer(dev,4);
    buffer[0]=index>>8;
    buffer[1]=index&0xFF;
    buffer[2]=data>>8;
    buffer[3]=data&0xFF;

    status=VL6180_I2CWrite(dev, buffer,(uint8_t)4);
    VL6180_DoneI2CAcces(dev);
    return status;
}

int VL6180_WrDWord(VL6180Dev_t dev, uint16_t index, uint32_t data){
    VL6180_I2C_USER_VAR
    DECL_I2C_BUFFER
    int  status;
    uint8_t *buffer;


    VL6180_GetI2CAccess(dev);
    buffer=VL6180_GetI2cBuffer(dev,6);
    buffer[0]=index>>8;
    buffer[1]=index&0xFF;
    buffer[2]=data>>24;
    buffer[3]=(data>>16)&0xFF;
    buffer[4]=(data>>8)&0xFF;;
    buffer[5]=data&0xFF;
    status=VL6180_I2CWrite(dev, buffer,(uint8_t)6);
    VL6180_DoneI2CAcces(dev);

    return status;
}

int VL6180_UpdateByte(VL6180Dev_t dev, uint16_t index, uint8_t AndData, uint8_t OrData){
    VL6180_I2C_USER_VAR
    int  status;
    uint8_t *buffer;
    DECL_I2C_BUFFER

    VL6180_GetI2CAccess(dev);

    buffer=VL6180_GetI2cBuffer(dev,3);
    buffer[0]=index>>8;
    buffer[1]=index&0xFF;

    status=VL6180_I2CWrite(dev, (uint8_t *)buffer,(uint8_t)2);
    if( !status ){
        /* read data direct onto buffer */
        status=VL6180_I2CRead(dev, &buffer[2],1);
        if( !status ){
            buffer[2]=(buffer[2]&AndData)|OrData;
            status=VL6180_I2CWrite(dev, buffer, (uint8_t)3);
        }
    }

    VL6180_DoneI2CAcces(dev);

    return status;
}

int VL6180_RdByte(VL6180Dev_t dev, uint16_t index, uint8_t *data){
    VL6180_I2C_USER_VAR
    int  status;
    uint8_t *buffer;
    DECL_I2C_BUFFER

    VL6180_GetI2CAccess(dev);

    buffer=VL6180_GetI2cBuffer(dev,2);
    buffer[0]=index>>8;
    buffer[1]=index&0xFF;

    status=VL6180_I2CWrite(dev, buffer, (uint8_t)2);
    if( !status ){
        status=VL6180_I2CRead(dev, buffer,1);
        if( !status ){
            *data=buffer[0];
        }
    }
    VL6180_DoneI2CAcces(dev);

    return status;
}

int VL6180_RdWord(VL6180Dev_t dev, uint16_t index, uint16_t *data){
    VL6180_I2C_USER_VAR
    int  status;
    uint8_t *buffer;
    DECL_I2C_BUFFER

    VL6180_GetI2CAccess(dev);

    buffer=VL6180_GetI2cBuffer(dev,2);
    buffer[0]=index>>8;
    buffer[1]=index&0xFF;

    status=VL6180_I2CWrite(dev, buffer, (uint8_t)2);
    if( !status){
        status=VL6180_I2CRead(dev, buffer,2);
        if( !status ){
            /* VL6180 register are Big endian if cpu is be direct read direct into *data is possible */
            *data=((uint16_t)buffer[0]<<8)|(uint16_t)buffer[1];
        }
    }
    VL6180_DoneI2CAcces(dev);
    return status;
}

int  VL6180_RdDWord(VL6180Dev_t dev, uint16_t index, uint32_t *data){
    VL6180_I2C_USER_VAR
    int status;
    uint8_t *buffer;
    DECL_I2C_BUFFER

    VL6180_GetI2CAccess(dev);
    buffer=VL6180_GetI2cBuffer(dev,4);

    buffer[0]=index>>8;
    buffer[1]=index&0xFF;

    status=VL6180_I2CWrite(dev, (uint8_t *) buffer, (uint8_t)2);
    if( !status ){
        status=VL6180_I2CRead(dev, buffer,4);
        if( !status ){
            /* VL6180 register are Big endian if cpu is be direct read direct into data is possible */
            *data=((uint32_t)buffer[0]<<24)|((uint32_t)buffer[1]<<16)|((uint32_t)buffer[2]<<8)|((uint32_t)buffer[3]);
        }
    }
    VL6180_DoneI2CAcces(dev);
    return status;
}


int  VL6180_RdMulti(VL6180Dev_t dev, uint16_t index, uint8_t *data, int nData){
    VL6180_I2C_USER_VAR
    int status;
    uint8_t *buffer;
    DECL_I2C_BUFFER

    VL6180_GetI2CAccess(dev);
    buffer=VL6180_GetI2cBuffer(dev,2);

    buffer[0]=index>>8;
    buffer[1]=index&0xFF;

    status=VL6180_I2CWrite(dev, (uint8_t *) buffer, (uint8_t)2);
    if( !status ){
        status=VL6180_I2CRead(dev, data, nData);
    }
    VL6180_DoneI2CAcces(dev);
    return status;
}