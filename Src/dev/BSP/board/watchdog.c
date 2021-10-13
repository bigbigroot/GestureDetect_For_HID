/**
 * @file watchdog.c
 * @author Weigen Huang (weigen.huang.k7e@fh-zwickau.de)
 * @brief 
 * @version 0.1
 * @date 2021-08-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_iwdg.h"


static IWDG_HandleTypeDef hiwdg;

void hardware_watchdog_init(int timeout)
{
    hiwdg.Instance = IWDG;
    /* clock frequence 32kHz */
    hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hiwdg.Init.Reload = timeout;
    hiwdg.Init.Window = 0;

    HAL_IWDG_Init(&hiwdg);
}

/**
 * @brief open watchdog
 * 
 */
void hardware_watchdog_start(void)
{
    __HAL_IWDG_START(&hiwdg);
}
/**
 * @brief feed watchdog
 * 
 */
void hardware_watchdog_feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}