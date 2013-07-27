/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef DISCOVERYF4_BOARD_H_
#define DISCOVERYF4_BOARD_H_

#include <platform/stm32f4/registers.h>
#include <platform/stm32f4/gpio.h>
#include <platform/stm32f4/usart.h>
#include <platform/stm32f4/nvic.h>
#include <platform/stm32f4/systick.h>

extern struct usart_dev console_uart;
extern struct usart_dev gdb_serial_uart;

#define BOARD_UART_DEVICE \
	UART4_IRQn

#endif	/* DISCOVERYF4_BOARD_H_ */
