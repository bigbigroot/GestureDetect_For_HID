#include <stdint.h>
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_uart.h"


extern UART_HandleTypeDef huart3;
/**
 * @brief send a character to terminal
 * 
 * @param ch character for send
 * @return int 
 */
int usart_putchar(int ch)
{
  uint8_t c = (uint8_t)ch;
  HAL_UART_Transmit(&huart3, &c, 1, 10);
  return ch;
}