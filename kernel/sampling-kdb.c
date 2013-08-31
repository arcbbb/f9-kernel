/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#if !defined(CONFIG_KPROBES)
#error __FILE__ " depends on CONFIG_KPROBES"
#endif

#include <ksym.h>
#include <sampling.h>
#include <types.h>
#include <thread.h>
#include <debug.h>
#include <lib/stdlib.h>
#include <kprobes.h>
#include <platform/cortex_m.h>

void sampling_ktimer(void)
{
	uint32_t *target_stack;
	uint32_t *stack = (uint32_t *)__builtin_frame_address(0)+6;
	uint32_t lr = *stack;

	if (lr & 0x4)
		target_stack = PSP();
	else
		target_stack = stack+1;
	sampled_pcpush((void *) target_stack[REG_PC]);
	return;
}

extern void ktimer_handler(void);
void kdb_show_sampling(void)
{
	int i;
	int symid;
	int *hitcount, *symid_list;
	static int init = 0;

	if (init == 0) {
		dbg_printf(DL_KDB, "Init sampling...\n");
		sampling_init();
		sampling_enable();
		init++;
		return;
	}

	sampling_disable();
	sampling_stats(&hitcount, &symid_list);

	for (i = 0; i < ksym_total(); i++) {
		symid = symid_list[i];
		if (hitcount[symid] == 0)
			break;
		dbg_printf(DL_KDB, "%5d [ %24s ]\n", hitcount[symid], 
				ksym_id2name(symid));
	}

	sampling_enable();
}
