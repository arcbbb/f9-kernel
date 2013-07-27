/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_GDB_UART_H_
#define PLATFORM_GDB_UART_H_
#include <types.h>

void gdb_uart_init();

void gdb_putchar(char chr);
uint8_t gdb_getchar();

#endif /* PLATFORM_GDB_UART_H_ */
