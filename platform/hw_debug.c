/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <platform/irq.h>
#include <platform/hw_debug.h>
#include <platform/cortex_m.h>

#ifdef CONFIG_KPROBES

extern void breakpoint_handler(uint32_t *stack);

static char fp_comp[FPB_MAX_COMP];
static int (*dwt_comp_func[5])(uint32_t *stack);

void hw_debug_init()
{
	int i;

	/* Enable FPB breakpoint */
	*FPB_CTRL = FPB_CTRL_KEY |FPB_CTRL_ENABLE ;

	/* Enable DWT watchpoint & DebugMon exception */
	*DCB_DEMCR |= DCB_DEMCR_TRCENA | DCB_DEMCR_MON_EN;

	/* Clear status bit */
	*SCB_HFSR = SCB_HFSR_DEBUGEVT;
	*SCB_DFSR = SCB_DFSR_BKPT;
	*SCB_DFSR = SCB_DFSR_HALTED;
	*SCB_DFSR = SCB_DFSR_DWTTRAP;

	for (i = 0; i < FPB_MAX_COMP; i++) {
		fp_comp[i] = 0;
	}
}

int get_avail_bkpt()
{
	int i;
	for (i = 0; i < FPB_MAX_COMP; i++) {
		if (fp_comp[i] == 0) {
			return i;
		}
	}
	return -1;
}

int breakpoint_install(uint32_t addr)
{
	int id = get_avail_bkpt();

	if (id < 0) {
		return -1;
	}

	fp_comp[id] = 1; /* Mark comparator allocated */
	if (addr & 2) {
		*(FPB_COMP + id) = FPB_COMP_REPLACE_UPPER|addr|FPB_COMP_ENABLE;
	} else {
		*(FPB_COMP + id) = FPB_COMP_REPLACE_LOWER|addr|FPB_COMP_ENABLE;
	}
	return id;
}

void breakpoint_uninstall(int id)
{
	fp_comp[id] = 0; /* Mark comparator free */
	*(FPB_COMP + id) &= ~FPB_COMP_ENABLE;
}

void watch_cycle_countdown(uint32_t cycle, int (*func)())
{
	dwt_comp_func[0] = func;
	DWT_COMP0_CYCLE(cycle);
}

void watchpoint_handler(uint32_t *stack)
{
	/* Examine whether COMP is matched */
	if ((*(DWT_COMP + 0)).func & DWT_FUNC_MATCHED) {
		dwt_comp_func[0]((void *)(stack));
		*SCB_DFSR  =  SCB_DFSR_DWTTRAP;
	}
}

void __debugmon_handler(uint32_t *stack)
{
	breakpoint_handler(stack);
	watchpoint_handler(stack);
}

void debugmon_handler() __NAKED;
void debugmon_handler()
{
	__asm__ __volatile__ ("push {lr}");
	/* select r0 - Main/Process Stack */
	__asm__ __volatile__ ("and r0, lr, #4");
	__asm__ __volatile__ ("cmp r0, #0");
	__asm__ __volatile__ ("itte eq");
	__asm__ __volatile__ ("mrseq r0, msp");
	__asm__ __volatile__ ("addeq r0, r0, #4");
	__asm__ __volatile__ ("mrsne r0, psp");
	__asm__ __volatile__ ("bl __debugmon_handler");
	__asm__ __volatile__ ("pop {pc}");
}

#endif /* CONFIG_KPROBES */
