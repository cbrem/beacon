#include "usart.h"

// NOTE: baud = F_CPU / (16 * (BAUD_PRESCALE + 1))
// For F_CPU = 2 * 10^6, BAUD_PRESCALE = 10 gives baud ~ 152000.
#define BAUD_PRESCALE 10

void usart_init() {
    // Enable receiver and transmitter.
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    // Set frame format.
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);

    // Set baud rate.
    UBRR0L = (uint8_t)BAUD_PRESCALE;
    UBRR0H = BAUD_PRESCALE >> 8;
}

void usart_putc(char c) {
    if (c == '\n') {
        usart_putc('\r');
    }

    // Wait for transmit buffer to empty, then set transmit buffer to c.
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

void usart_puts(char* s) {
    char c;
    while(c = *(s++)) {
        usart_putc(c);
    }
}

char usart_getc() {
    // Wait until the receive buffer is full, then return its value.
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}
