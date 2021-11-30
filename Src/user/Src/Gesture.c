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
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "timers.h"
#include "stm32f7xx_hal.h"
#include "6180a1.h"
#include "vl6180_api.h"
#include "vl6180_i2c.h"
#include "vl6180_platform.h"
#include "vl6180_types.h"
#include "Gesture.h"
#include "sim_mouse.h"
#include "led.h"
#include "watchdog.h"

extern TimerHandle_t blinkTimer;
extern I2C_HandleTypeDef hi2c1;
extern int x;
extern int y;

#define TOP 0
#define LEFT 1
#define BOTTOM 2
#define RIGHT 3

#define TASK_DELAY(ms) vTaskDelay((ms*1000)/configTICK_RATE_HZ);


char DisplayStr[5];
const char DecToAsciiVec[10]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

VL6180Dev_t theVL6180Dev;
struct MyVL6180Dev_t BoardDevs[4] = { [0]= { .DevID = 0 }, [1]= { .DevID = 1 }, [2]= { .DevID = 2 }, [3]= { .DevID = 3 }, };
VL6180_RangeData_t measureRanges[4];
int nPresent;
int nReady;
/********************************************************************************/ 
/* GESTURES : Variables for Gestures recognition                                */
/********************************************************************************/
static GestureSysStateTyp GestureSysState=INIT; // Start demo with ranging
ButtonState buttonState = RELEASE;
int detectedN;
int avgRange;
void HandleError(const char *msg);
void sendDisplayStr(char *str);


/**
 * @brief Gesture Detector
 * 
 * @param pvParameters 
 */
void GestureDetectorHandler(void *pvParameters)
{
    int i;
    int status;
    int n_dev=4;
    int nPresentDevs;
    int idleCnt=0;

    MX_USB_DEVICE_Init();
    
    LED_OFF(LED1);
    for (i = 0; i < n_dev; i++) 
    {
        /* put all under reset */
        XNUCLEO6180XA1_ResetId(0, i);
    }
  

    /* detect presence and initialize devices i2c address  */
    /*set device i2c address for dev[i] = 0x52+(i+1)*2 */
    nPresentDevs = 0;
    //for (i = n_dev - 1; i >= 0; i--) {
    for (i=0;i<n_dev;i++) {
        int FinalI2cAddr;
        uint8_t id;
        
        /* unreset device that wake up at default i2c addres 0x52 */
        XNUCLEO6180XA1_ResetId(1, i);
        TASK_DELAY(2);    /* at least 400usec before to acces device */
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
                HandleError("VL6180_SetI2CAddress fail\r\n");
            }else{
                printf("Set ToF proximity Device%d: Address: 0x%x\r\n", i, FinalI2cAddr);
            }
            BoardDevs[i].I2cAddr = FinalI2cAddr;
        }

        TASK_DELAY(1);
        status = VL6180_RdByte(&BoardDevs[i], IDENTIFICATION_MODEL_ID, &id);
        TASK_DELAY(1);
        status= VL6180_Prepare(&BoardDevs[i]);
        if( status<0 ){
            HandleError("VL6180_Prepare fail\r\n");
        }
        /* Set scaling factor to 1 */
        VL6180_UpscaleSetScaling(&BoardDevs[i], 1);
        /* Disable Dmax computation */
        VL6180_DMaxSetState(&BoardDevs[i], 0);
    }
    
    printf("Gesture Detect start working...\r\n");
    LED_ON(LED1);
    TASK_DELAY(1);
    

    //hardware_watchdog_init(1000);
    while (1)
    {
        //hardware_watchdog_feed();
        detectedN =0;
        avgRange = 0;

        
        if(GestureSysState == IDLE) TASK_DELAY(20);

        for(i=0; i<n_dev; i++)
        {
            if(BoardDevs[i].Present){
                status = VL6180_RangeStartSingleShot(&BoardDevs[i]);

                if(status <0)
                {
                     printf("Dev%d StartSingleShot fail!\r\n", i);
                }
            }
        }

        TASK_DELAY(2); 
        for (i = 0; i < n_dev; i++)
        {
            int mTimes =  10; 
            BoardDevs[i].Ready=1;
            do{              
                VL6180_RangeGetMeasurementIfReady(&BoardDevs[i], &measureRanges[i]);
                if(measureRanges[i].errorStatus == DataNotReady) 
                {
                    TASK_DELAY(1); 
                }
                else break;
            }while(mTimes-- > 0);
            if(mTimes>0){
                if(MeasureErrHandle(i, measureRanges[i].errorStatus) == Detected){
                    detectedN++;
                    avgRange += measureRanges[i].range_mm;
                    //printf("Dev%d: %d mm\r\n", i, measureRanges[i].range_mm);
                }
            }
            else{
                BoardDevs[i].Ready=0;
            }
        }
        
        //continue;
        if(detectedN>0){
            avgRange /=detectedN;
            if(avgRange <50 && buttonState==RELEASE)
            {
                buttonState = PRESS;
            }else if(avgRange >90 && buttonState==PRESS)
            {
                buttonState = RELEASE;
            }
        }


        switch(GestureSysState){
            case INIT:
                buttonState = RELEASE;
                GestureSysState = RANGE;
#if 0
            case RANGE:
                if (BoardDevs[TOP].Ready && measureRanges[TOP].errorStatus == 0){                    
                    if(detectedN == 1)
                        GestureSysState = TOP_TO_BOTTOM_SWIPE;
                }
                else if(BoardDevs[LEFT].Ready && measureRanges[LEFT].errorStatus == 0 ){
                    if(detectedN == 1)
                        GestureSysState = LEFT_TO_RIGHT_SWIPE;
                    
                }
                else if(BoardDevs[RIGHT].Ready && measureRanges[RIGHT].errorStatus == 0 ){
                    if(detectedN == 1)
                        GestureSysState = RIGHT_TO_LEFT_SWIPE;
                }
                else if(BoardDevs[BOTTOM].Ready && measureRanges[BOTTOM].errorStatus == 0 )
                {                    
                    if(detectedN == 1)
                        GestureSysState = BOTTOM_TO_TOP_SWIPE;
                }
                
                break;
            case RIGHT_TO_LEFT_SWIPE:
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                if(measureRanges[LEFT].errorStatus == 0){
                    GestureSysState = LEFT_TO_RIGHT_SWIPE;
                }
                break;
            case LEFT_TO_RIGHT_SWIPE:
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                if(measureRanges[RIGHT].errorStatus == 0){
                    GestureSysState = RIGHT_TO_LEFT_SWIPE;
                }
                break;
            case TOP_TO_BOTTOM_SWIPE:
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                if(measureRanges[RIGHT].errorStatus == 0){
                    GestureSysState = RIGHT_TO_LEFT_SWIPE;
                }
                break;
            case BOTTOM_TO_TOP_SWIPE:
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                if(measureRanges[RIGHT].errorStatus == 0){
                    GestureSysState = RIGHT_TO_LEFT_SWIPE;
                }
                break;
#endif       
            case IDLE:
            case RANGE:  
                if(GestureSysState==IDLE)
                {
                    if(detectedN>0)
                    {
                        printf("System will wake up.\r\n");
                        xTimerStop(blinkTimer, 0);
                        LED_ON(LED3);
                        idleCnt=0;
                    }
                }else
                {
                    if(detectedN==0)
                    {
                        if(idleCnt>10000)
                        {
                            GestureSysState=IDLE;
                            printf("System will sleep...\r\n");
                            xTimerStart(blinkTimer, 0);
                        }else
                        {
                            idleCnt++;
                        }                                          
                    }else
                    {
                        idleCnt = 0;
                    }                                        
                }
                
                if (isOnTheTop())
                {       
                    GestureSysState = ON_THE_TOP;
                }
                else if(isOnTheLeft())
                {
                    GestureSysState = ON_THE_LEFT;                        
                }
                else if(isOnTheRight())
                {
                    GestureSysState = ON_THE_RIGHT;
                }
                else if(isOnTheBottom())
                {                    
                    GestureSysState = ON_THE_BOTTOM;
                }
                else if(isOnTheMid())
                {                    
                    GestureSysState = ON_THE_MID;
                }
                break;    
            case ON_THE_MID:                
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                else if (isOnTheTop()){       
                    GestureSysState = ON_THE_TOP;
                    mouseMoveAndClick(buttonState, 0, -25);
                }
                else if(isOnTheLeft()){
                    GestureSysState = ON_THE_LEFT;
                    mouseMoveAndClick(buttonState, -25, 0);
                }
                else if(isOnTheRight()){
                    GestureSysState = ON_THE_RIGHT;
                    mouseMoveAndClick(buttonState, 25, 0);
                }
                else if(isOnTheBottom())
                {                    
                    GestureSysState = ON_THE_BOTTOM;
                    mouseMoveAndClick(buttonState, 0, 25);
                }
                break;
            case ON_THE_TOP:                
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                else if(isOnTheLeft()){
                    GestureSysState = ON_THE_LEFT;
                    mouseMoveAndClick(buttonState, -25, 25);
                    
                }
                else if(isOnTheRight()){
                    GestureSysState = ON_THE_RIGHT;
                    mouseMoveAndClick(buttonState, 25, 25);
                }
                else if(isOnTheBottom())
                {                    
                    GestureSysState = ON_THE_BOTTOM;
                    mouseMoveAndClick(buttonState, 0, 50);
                }
                else if(isOnTheMid())
                {                    
                    GestureSysState = ON_THE_MID;
                    mouseMoveAndClick(buttonState, 0, 25);
                }
                break;
            case ON_THE_BOTTOM:                
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                else if (isOnTheTop()){       
                    GestureSysState = ON_THE_TOP;
                    mouseMoveAndClick(buttonState, 0, -50);
                }
                else if(isOnTheLeft()){
                    GestureSysState = ON_THE_LEFT;
                    mouseMoveAndClick(buttonState, -25, -25);
                    
                }
                else if(isOnTheRight()){
                    GestureSysState = ON_THE_RIGHT;
                    mouseMoveAndClick(buttonState, 25, -25);
                }
                else if(isOnTheMid())
                {                    
                    GestureSysState = ON_THE_MID;
                    mouseMoveAndClick(buttonState, 0, -25);
                }
                break;
            case ON_THE_LEFT:                
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                else if (isOnTheTop()){       
                    GestureSysState = ON_THE_TOP;
                    mouseMoveAndClick(buttonState, 25, -25);
                }
                else if(isOnTheRight()){
                    GestureSysState = ON_THE_RIGHT;
                    mouseMoveAndClick(buttonState, 50, 0);
                }
                else if(isOnTheBottom())
                {                    
                    GestureSysState = ON_THE_BOTTOM;
                    mouseMoveAndClick(buttonState, 25, 25);
                }
                else if(isOnTheMid())
                {                    
                    GestureSysState = ON_THE_MID;
                    mouseMoveAndClick(buttonState, 25, 0);
                }
                break;
            case ON_THE_RIGHT:                
                if(detectedN==0){
                    GestureSysState  = RANGE;
                }
                else if (isOnTheTop()){       
                    GestureSysState = ON_THE_TOP;
                    mouseMoveAndClick(buttonState, -25, -25);
                }
                else if(isOnTheLeft()){
                    GestureSysState = ON_THE_LEFT;
                    mouseMoveAndClick(buttonState, -50, 0);
                    
                }
                else if(isOnTheBottom())
                {                    
                    GestureSysState = ON_THE_BOTTOM;
                    mouseMoveAndClick(buttonState, -25, 25);
                }
                else if(isOnTheMid())
                {                    
                    GestureSysState = ON_THE_MID;
                    mouseMoveAndClick(buttonState, -25, 0);
                }
                break;
            default:
                GestureSysState = INIT;  
        }


    }
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
    TASK_DELAY(SegDelayMs);
}

void sendDisplayStr(char *str)
{
    strcpy(DisplayStr, str);
}
void DisplayExec()
{
    int len;
    len=strlen(DisplayStr);
    if(len < 4)
        XNUCLEO6180XA1_DisplayString(DisplayStr, len);
    else
        XNUCLEO6180XA1_DisplayString(DisplayStr,4);
}

void HandleError(const char *msg) {
    static char ErrString[256]="Err ";
    strcat(ErrString,msg);
    printf("%s\r\n", ErrString);
    sendDisplayStr("Err");
    while (1);
}

char isOnTheLeft()
{
    return BoardDevs[LEFT].Ready 
        && measureRanges[LEFT].errorStatus == 0 
        && detectedN == 1; 
}
char isOnTheRight()
{
    return BoardDevs[RIGHT].Ready 
        && measureRanges[RIGHT].errorStatus == 0 
        && detectedN == 1; 
}
char isOnTheTop()
{
    return BoardDevs[TOP].Ready 
        && measureRanges[TOP].errorStatus == 0 
        && detectedN == 1; 
}
char isOnTheBottom()
{
    return BoardDevs[RIGHT].Ready 
        && measureRanges[BOTTOM].errorStatus == 0 
        && detectedN <3; 
}
char isOnTheMid()
{
    return detectedN > 2; 
}

RangeStatusOrErrorTyp MeasureErrHandle(int dev, int errorCode)
{
    RangeStatusOrErrorTyp ret;

    switch (errorCode)
    {
    case 0:
        ret = Detected;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        ret = NeedToReset;
        break;
    case 6:
    case 7:
        ret = NotDetected;
        break;
    case 8:
        break;
    case 11:
        ret = Noise;
        break;
    case 12:
    case 14:
        ret = TooLow;
        break;
    case 13:
    case 15:
        ret = TooHigh;
        break;
    case 16:
        break;
    case 18:
        ret = DataNotReady;
        break;    
    case 9:
    case 10:
    default:
        printf("ToF proximity Device%d: Unknow error code %d.\r\n", dev, errorCode);
        ret = UnknowError;
        break;
    }

    return ret;
}