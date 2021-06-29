#include "stm32f7xx_hal_iwdg.h"


static IWDG_HandleTypeDef hwdg;

void hardware_watchdog_init(int timeout)
{
    hwdg.Instance = IWDG;
    /* clock frequence 32kHz */
    hwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hwdg.Init.Reload = timeout*1000;
    hwdg.Init.Window = 0;

    HAL_IWDG_Init(&hwdg);
}

void hardware_watchdog_deinit(int timeout)
{
    hwdg.Instance = IWDG;
    /* clock frequence 32kHz */
    hwdg.Init.Prescaler = IWDG_PRESCALER_32;
    hwdg.Init.Reload = timeout*1000;
    hwdg.Init.Window = 0;

    HAL_IWDG_Init(&hwdg);
}
/**
 * @brief feed watchdog
 * 
 */
void hardware_watchdog_enable(void)
{
    __HAL_IWDG_START(&hiwdg);
}
/**
 * @brief feed watchdog
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