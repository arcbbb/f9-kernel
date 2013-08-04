/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <kprobes.h>
#include <debug.h>

extern void ktimer_handler();
extern void svc_handler();


static struct kprobe kp_kt;
static int kt_count, kt_adv_count;
static int kp_kt_prehandler(struct kprobe *kp, struct kp_regs *target)
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
	kp_kt.addr = ktimer_handler;
	kp_kt.pre_handler = kp_kt_prehandler;
	kp_kt.post_handler = NULL;
	kprobe_register(&kp_kt);
}

#include <softirq.h>
static struct kprobe kp_softirq;
static uint32_t softirq_stats[NR_SOFTIRQ];
static int kp_softirq_prehandler(struct kprobe *kp, struct kp_regs *target)
{
	softirq_type_t type = target->regs[0];
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
	int i;
	for (i = 0; i < NR_SOFTIRQ; i++) {
		softirq_stats[i] = 0;
	}
	kp_softirq.addr = softirq_schedule;
	kp_softirq.pre_handler = kp_softirq_prehandler;
	kp_softirq.post_handler = NULL;
	kprobe_register(&kp_softirq);

}

static struct kprobe kp_svc;
static int svc_count;
int kp_svc_posthandler(struct kprobe *kp, struct kp_regs *target)
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
	kp_svc.addr = svc_handler;
	kp_svc.pre_handler = NULL;
	kp_svc.post_handler = kp_svc_posthandler;
	kprobe_register(&kp_svc);
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
	}
	else {
		kp_kt_stat();
		kp_svc_stat();
		kp_softirq_stat();
	}
}
