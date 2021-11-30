/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "usb_device.h"
#include "usbd_desc.h"
#include "usbd_core.h"
#include "usbd_hid.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Gesture.h"
/**
 * @brief user button's state
 *        true: press
 *        false: release
 * 
 */
extern ButtonState buttonState;


USBD_HandleTypeDef hUsbDeviceFS;


/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
  {
    Error_Handler("usbd init fault");
    LED_OFF(LED2);
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_HID) != USBD_OK)
  {
    Error_Handler("cannot Register Class");
    LED_OFF(LED2);
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
  {
    Error_Handler("cannot start usbd");
    LED_OFF(LED2);
  }
}

#if 0

static uint8_t *USBD_HID_GetPos(void)
{
  if(buttonState==PRESS)
  {
    HID_Buffer[0] = LeftButton;
  }else if(buttonState==RELEASE){
    HID_Buffer[0] = 0;
  }
  // if(key1_press){
  //   HID_Buffer[0] = 1;
  //   key1_press=0;
  // }else{
  //   HID_Buffer[0] = 0;
  // }
  HID_Buffer[1] = x;
  HID_Buffer[2] = y;
  HID_Buffer[3] = 0;
  return HID_Buffer;
}

/**
  * @brief  Function implementing the usb thread.
  * @param  argument: Not used
  * @retval None
  */
void USBDeviceReportHandler(void *pvParameters)
{
  TickType_t xLastWakeTime;

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* Infinite loop */
  while(1)
  {
    vTaskDelay(100000);
    ulTaskNotifyTake(pdTRUE, 10000);
    USBD_HID_SendReport(&hUsbDeviceFS, USBD_HID_GetPos(), 4);
    x=0;
    y=0;
  }
}
#endif
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
