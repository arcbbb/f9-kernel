/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <kprobes.h>
#include <debug.h>
#include <platform/armv7m.h>

extern void ktimer_handler();
extern void svc_handler();


static int kt_count, kt_adv_count;
static int kp_kt_prehandler(struct kprobe *kp, uint32_t *stack, uint32_t *regs)
{
	kt_count++;
	if (kt_count == 10000) {
		kt_count = 0;
		kt_adv_count++;
	}
	return 0;
}

static void kp_kt_stat()
{
	dbg_printf(DL_KDB, "ktimer = %d, %d\n", kt_count, kt_adv_count);
}

static void kp_kt_init()
{
	static struct kprobe kp_kt;
	kp_kt.addr = ktimer_handler;
	kp_kt.pre_handler = kp_kt_prehandler;
	kp_kt.post_handler = NULL;
	kprobe_register(&kp_kt);
}

#include <softirq.h>
static uint32_t softirq_stats[NR_SOFTIRQ];
static int kp_softirq_prehandler(struct kprobe *kp, uint32_t *stack,
				 uint32_t *regs)
{
	softirq_type_t type = stack[REG_R0];
	softirq_stats[type]++;
	return 0;
}

static void kp_softirq_stat()
{
	int i;
	const char *softirq_str[] = {
		"KTE_SOFTIRQ","ASYNC_SOFTIRQ",
		"SYSCALL_SOFTIRQ","KDB_SOFTIRQ"};

	for (i = 0; i < NR_SOFTIRQ; i++) {
		dbg_printf(DL_KDB, "%s = %d times\n",
				softirq_str[i],
				softirq_stats[i]);
	}
}

static void kp_softirq_init()
{
	static struct kprobe kp_softirq;
	int i;
	for (i = 0; i < NR_SOFTIRQ; i++) {
		softirq_stats[i] = 0;
	}
	kp_softirq.addr = softirq_schedule;
	kp_softirq.pre_handler = kp_softirq_prehandler;
	kp_softirq.post_handler = NULL;
	kprobe_register(&kp_softirq);

}

static int svc_count;
int kp_svc_posthandler(struct kprobe *kp, uint32_t *stack, uint32_t *regs)
{
	svc_count++;
	return 0;
}

static void kp_svc_stat()
{
	dbg_printf(DL_KDB, "SVCall = %d times\n", svc_count);
}

static void kp_svc_init()
{
	static struct kprobe kp_svc;
	kp_svc.addr = svc_handler;
	kp_svc.pre_handler = NULL;
	kp_svc.post_handler = kp_svc_posthandler;
	kprobe_register(&kp_svc);
}

static int globalid_count;
static int tcb;
static int globalid_handler(struct kprobe *kp, uint32_t *stack, uint32_t *regs)
{
	globalid_count++;
	tcb = stack[REG_R0];
	return 0;
}

static void globalid_stat()
{
	dbg_printf(DL_KDB, "thread_by_globalid hit = %d times\n", globalid_count);
	dbg_printf(DL_KDB, "tcb: 0x%08x\n", tcb);
}

void thread_by_globalid();
static void globalid_init()
{
	static struct kretprobe rp;

	rp.kp.addr = thread_by_globalid;
	rp.handler = globalid_handler;
	kretprobe_register(&rp);
}

void kdb_show_kprobe_info()
{
	static int init = 0;
	if (init == 0) {
		init ++;
		dbg_puts("Init kprobe test...\n");
		kp_kt_init();
		kp_svc_init();
		kp_softirq_init();
		globalid_init();
	}
	else {
		kp_kt_stat();
		kp_svc_stat();
		kp_softirq_stat();
		globalid_stat();
	}
}
