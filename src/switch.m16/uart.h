#define F_CPU 8000000UL
#define BAUD  9600
#define UBRR_VALUE F_CPU / 16 / BAUD - 1

#include <util/setbaud.h>

void UART_init();
void UART_puti(const int val);
void UART_putc(unsigned char data);
unsigned char UART_getc (void);
void UART_puts(char *str);
