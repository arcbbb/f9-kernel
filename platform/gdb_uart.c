/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <platform/gdb_uart.h>

/* board speficic UART definitions */
#include "board.h"

void gdb_uart_init()
{
	usart_init(&gdb_serial_uart);	
}

uint8_t gdb_getchar()
{
	uint8_t chr;

	while (!usart_status(&gdb_serial_uart, USART_RXNE));
	chr = usart_getc(&gdb_serial_uart);

	return chr;
}

static void gdb_sync_putchar(char chr);

void gdb_putchar(char chr)
{
	gdb_sync_putchar(chr);
}

static void gdb_sync_putchar(char chr)
{
	if (chr == '\n')
		gdb_sync_putchar('\r');

	/* read TC bit to clear it */
	usart_status(&gdb_serial_uart, USART_TC);
	usart_putc(&gdb_serial_uart, chr);
	while (!usart_status(&gdb_serial_uart, USART_TC));
}
