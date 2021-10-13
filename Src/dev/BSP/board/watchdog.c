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
    hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
    hiwdg.Init.Reload = timeout*5;
    hiwdg.Init.Window = 0;

    HAL_IWDG_Init(&hiwdg);
}

/**
 * @brief open watchdog
 * 
 */
void hardware_watchdog_enable(void)
{
    __HAL_IWDG_START(&hiwdg);
}
/**
 * @brief close watchdog
 * 
 */
void hardware_watchdog_disable(void)
{
    __HAL_IWDG_START(&hiwdg);
}
/**
 * @brief feed watchdog
 * 
 */
void hardware_watchdog_feed(void)
{
    __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
}