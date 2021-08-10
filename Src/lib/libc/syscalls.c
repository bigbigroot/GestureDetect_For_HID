/**
 * @file syscalls.c
 * @author Weigen Huang (weigen.huang.k7e@fh-zwickau.de)
 * @brief hardware's api for libc
 * @version 0.1
 * @date 2021-08-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "tty.h"

int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for(DataIdx= 0; DataIdx< len; DataIdx++)
  {
    usart_putchar(*ptr++);
  }
  return len;
}