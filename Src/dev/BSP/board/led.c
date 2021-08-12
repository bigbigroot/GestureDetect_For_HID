#include "board.h"
#include "led.h"
#include "stm32f7xx_hal.h"

void BSP_Init(){
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /*Configure push button GPIO pin : PC13  */
    __GPIOC_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void LED_ON(LEDNum n)
{
    switch (n)
    {
    case LED1:
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
        break;
    case LED2:
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);        
        break;    
    case LED3:
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);        
        break;    
    default:
        break;
    }
}

void LED_OFF(LEDNum n)
{
    switch (n)
    {
    case LED1:
        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
        break;
    case LED2:
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);        
        break;    
    case LED3:
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);        
        break;    
    default:
        break;
    }
}