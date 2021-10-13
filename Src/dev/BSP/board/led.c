#include "board.h"
#include "led.h"
#include "stm32f7xx_hal.h"


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

void LED_BLINK(LEDNum n)
{
    switch (n)
    {
    case LED1:
        HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
        break;
    case LED2:
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);        
        break;    
    case LED3:
        HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);        
        break;    
    default:
        break;
    }
}