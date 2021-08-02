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
#include "vl6180_i2c.h"
#include "vl6180_platform.h"
#include "vl6180_types.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief Gesture Detector
 * 
 * @param pvParameters 
 */
void GestureDetectorHandler(void *pvParameters)
{

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