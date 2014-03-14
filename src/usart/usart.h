#ifndef _USART_H
#define _USART_H

#include <avr/io.h>
#include <stdlib.h>

void usart_init();
void usart_putc(char c);
void usart_puts(char* s);
char usart_getc();

#endif _USART_H
