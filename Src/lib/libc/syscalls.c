
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for(DataIdx= 0; DataIdx< len; DataIdx++)
  {
    usart_putchar(*ptr++);
  }
  return len;
}