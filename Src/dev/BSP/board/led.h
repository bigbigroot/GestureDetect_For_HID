/**
 * @file led.h
 * @author Weigen Huang (weigen.huang.k7e@fh-zwickau.de)
 * @brief 
 * @version 0.1
 * @date 2021-08-12
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef LED_H_
#define LED_H_

typedef enum LEDNum_typ
{
    LED1=1,
    LED2,
    LED3
} LEDNum;

void LED_ON(LEDNum n);
void LED_OFF(LEDNum n);

#endif