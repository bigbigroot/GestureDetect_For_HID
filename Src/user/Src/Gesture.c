/**
 * @file Gesture.c
 * @author Weigen Huang (weigen.huang.k7e@fh-zwickau.de)
 * @brief  
 * @version 0.1
 * @date 2021-07-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "stm32f7xx_hal.h"
#include "6180a1.h"
#include "vl6180_api.h"
#include "vl6180_i2c.h"
#include "vl6180_platform.h"
#include "vl6180_types.h"

extern I2C_HandleTypeDef hi2c1;


VL6180Dev_t theVL6180Dev;
struct MyVL6180Dev_t BoardDevs[4] = { [0]= { .DevID = 0 }, [1]= { .DevID = 1 }, [2]= { .DevID = 2 }, [3]= { .DevID = 3 }, };
VL6180Dev_t theVL6180Dev = &BoardDevs[0];

/**
 * @brief Gesture Detector
 * 
 * @param pvParameters 
 */
void GestureDetectorHandler(void *pvParameters)
{
    int status;
    uint8_t id;
    
    BoardDevs[0].I2cAddr = 0x52;
    /* to detect device presence try to read it's dev id */
    status = VL6180_RdByte(&BoardDevs[0], IDENTIFICATION_MODEL_ID, &id);

}

#define i2c_bus      (&hi2c1)
#define def_i2c_time_out 100

int VL6180_I2CWrite(VL6180Dev_t dev, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Transmit(i2c_bus, dev->I2cAddr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }
    return status? -1 : 0;
}

int VL6180_I2CRead(VL6180Dev_t dev, uint8_t *buff, uint8_t len) {
    int status;
    status = HAL_I2C_Master_Receive(i2c_bus, dev->I2cAddr, buff, len, def_i2c_time_out);
    if (status) {
        XNUCLEO6180XA1_I2C1_Init(&hi2c1);
    }

    return status? -1 : 0;
}