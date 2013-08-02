/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "board.h"

void irq_init()
{
	/* Set all 4-bit to pre-emption priority bit */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Set default priority */
	NVIC_SetPriority(DebugMonitor_IRQn, 0x0, 0);

	NVIC_SetPriority(MemoryManagement_IRQn, 0x1, 0);
	NVIC_SetPriority(BusFault_IRQn, 0x1, 0);
	NVIC_SetPriority(UsageFault_IRQn, 0x1, 0);

	/* priority 0x2 for uart console */

	NVIC_SetPriority(SysTick_IRQn, 0x3, 0);

	NVIC_SetPriority(SVCall_IRQn, 0xF, 0);
	NVIC_SetPriority(PendSV_IRQn, 0xF, 0);
}

