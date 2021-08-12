/**
 * @file tty.h
 * @author Weigen Huang (weigen.huang.k7e@fh-zwickau.de)
 * @brief 
 * @version 0.1
 * @date 2021-08-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef TTY_H
#define TTY_H


/**
 * @brief send a character to terminal
 * 
 * @param ch character for send
 * @return int 
 */
int usart_putchar(int ch);

#endif /*TTY_H*/