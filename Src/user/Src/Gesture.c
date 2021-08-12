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

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "stm32f7xx_hal.h"
#include "6180a1.h"
#include "vl6180_api.h"
#include "vl6180_i2c.h"
#include "vl6180_platform.h"
#include "vl6180_types.h"

extern I2C_HandleTypeDef hi2c1;

char DisplayStr[5];
VL6180Dev_t theVL6180Dev;
struct MyVL6180Dev_t BoardDevs[4] = { [0]= { .DevID = 0 }, [1]= { .DevID = 1 }, [2]= { .DevID = 2 }, [3]= { .DevID = 3 }, };



/**
 * @brief Gesture Detector
 * 
 * @param pvParameters 
 */
void GestureDetectorHandler(void *pvParameters)
{
    int i;
    int status;
    int n_dev=1;
    int nPresentDevs;
    uint8_t id;
    
    BoardDevs[0].I2cAddr = 0x52;

    /* reset ranging devices */
    if (!XNUCLEO6180XA1_IsV2()) {
        XNUCLEO6180XA1_Reset(0);
        strcpy(DisplayStr,"T   ");
    }else{
        for (i = 0; i < n_dev; i++) {
            /* put all under reset */
            XNUCLEO6180XA1_ResetId(0, i);
        }
        strcpy(DisplayStr,"TLBR");
    }

    XNUCLEO6180XA1_DisplayString(DisplayStr, 4);

    /* detect presence and initialize devices i2c address  */
    /*set device i2c address for dev[i] = 0x52+(i+1)*2 */
    nPresentDevs = 0;
    //for (i = n_dev - 1; i >= 0; i--) {
    for (i=0;i<n_dev;i++) {
        int FinalI2cAddr;
        uint8_t id;
        
        /* unreset device that wake up at default i2c addres 0x52 */
        if (!XNUCLEO6180XA1_IsV2()) {
            XNUCLEO6180XA1_Reset(1);
        }else{
            XNUCLEO6180XA1_ResetId(1, i);
        }
        vTaskDelay(2);    /* at least 400usec before to acces device */
        BoardDevs[i].I2cAddr = 0x52;
        /* to detect device presence try to read it's dev id */
        status = VL6180_RdByte(&BoardDevs[i], IDENTIFICATION_MODEL_ID, &id);
        if (status) {
            /* these device is not present skip init and clear it's letter on string */
            BoardDevs[i].Present = 0;
            DisplayStr[i]=' ';
            continue;
        }
        /* device present only */
        BoardDevs[i].Present = 1;
        nPresentDevs++;
        status = VL6180_InitData(&BoardDevs[i]);

        FinalI2cAddr = 0x52 + ((i+1) * 2);
        if (FinalI2cAddr != 0x52) {
            status = VL6180_SetI2CAddress(&BoardDevs[i], FinalI2cAddr);
            if( status ){
                HandleError("VL6180_SetI2CAddress fail");
            }
            BoardDevs[i].I2cAddr = FinalI2cAddr;
        }

        vTaskDelay(1);
        status = VL6180_RdByte(&BoardDevs[i], IDENTIFICATION_MODEL_ID, &id);
        vTaskDelay(1);
        status= VL6180_Prepare(&BoardDevs[i]);
        if( status<0 ){
            HandleError("VL6180_Prepare fail");
        }
        /* Set scaling factor to 1 */
        VL6180_UpscaleSetScaling(&BoardDevs[i], 1);
        /* Disable Dmax computation */
        VL6180_DMaxSetState(&BoardDevs[i], 0);
    }

    VL6180_WaitDeviceBooted(&BoardDevs[0]);

    if(VL6180_InitData(&BoardDevs[0])) 
        printf("TOF proximity Device 0: InitData is unsuccssfull");

    
    if(VL6180_Prepare(&BoardDevs[0])) 
        printf("TOF proximity Device 0: VL6180 Prepare is unsuccssfull");
        
    /* to detect device presence try to read it's dev id */
    status = VL6180_RdByte(&BoardDevs[0], IDENTIFICATION_MODEL_ID, &id);
    printf("id: %d", BoardDevs[0].DevID);
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

void XNUCLEO6180XA1_WaitMilliSec(int SegDelayMs)
{
    vTaskDelay(SegDelayMs);
}

void HandleError(const char *msg) {
    static char ErrString[256]="Err ";
    strcat(ErrString,msg);
    printf(ErrString);
    while (1);
}