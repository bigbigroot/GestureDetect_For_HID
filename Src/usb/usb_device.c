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
/**
 * @brief user button's state
 *        true: press
 *        false: release
 * 
 */
extern int usr_btn_state;

/* Private variables ---------------------------------------------------------*/
static uint8_t HID_Buffer [4];
static int x=0;
static int y=255;
/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

static uint8_t *USBD_HID_GetPos(void)
{
  HID_Buffer[0] = 0;
  HID_Buffer[1] = x;
  HID_Buffer[2] = y;
  HID_Buffer[3] = 0;
  return HID_Buffer;
}

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_USB_DEVICE_Init(void)
{
  /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */

  /* USER CODE END USB_DEVICE_Init_PreTreatment */

  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
  {
    Error_Handler("usbd init fault");
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_HID) != USBD_OK)
  {
    Error_Handler("cannot Register Class");
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
  {
    Error_Handler("cannot start usbd");
  }

  /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */

  /* USER CODE END USB_DEVICE_Init_PostTreatment */
}


/**
  * @brief  Function implementing the usb thread.
  * @param  argument: Not used
  * @retval None
  */
void USBDeviceReportHandler(void *pvParameters)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();


  /* Infinite loop */
  while(1)
  {
    USBD_HID_GetPos();
    //USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 4);
    vTaskDelay(100);
  }
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
