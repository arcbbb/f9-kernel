/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <ksym.h>
#include <sampling.h>
#include <debug.h>
#include <lib/stdlib.h>

static void *sampled_pc[MAX_SAMPLING_COUNT];
static int sampled_count;
static int sampled_cycle;
static int __sampling_enabled;

static int ksym_hitcount[MAX_KSYM];
static int ksymid_sortedlist[MAX_KSYM];

void sampling_enable()
{
	__sampling_enabled = 1;
}

void sampling_disable()
{
	__sampling_enabled = 0;
}

void sampling_init()
{
	if (ksym_total() > MAX_KSYM) {
		dbg_printf(DL_KDB, "ksym %d > MAX_KSYM\n", ksym_total());
		return;
	}
	sampled_count = 0;
	sampled_cycle = 0;
}

int sampled_pcpush(void *pc)
{
	/* If sampling is disabled, return as if success */
	if (__sampling_enabled == 0)
		return 0;

	if (sampled_count == MAX_SAMPLING_COUNT) {
	       if (sampled_cycle == 0) {
		       sampled_count = 0;
		       sampled_cycle++;
	       }
	       else {
		       __sampling_enabled = 0;
		       return -1;
	       }
	}

	sampled_pc[sampled_count++] = pc;
	return 0;
}

static int cmp_addr(const void *p1, const void *p2)
{
	return *(int *) p1 - *(int *) p2;
}

static int cmp_symhit(const void *p1, const void *p2)
{
	int *symid1 = (int *) p1;
	int *symid2 = (int *) p2;
	return ksym_hitcount[*symid2] - ksym_hitcount[*symid1];
}

void sampling_stats(int **hitcountp, int **symid_list)
{
	int i,symid;

	/* init data */
	sort(sampled_pc, MAX_SAMPLING_COUNT, sizeof(sampled_pc[0]), cmp_addr);

	for (i = 0; i < MAX_KSYM; i++) {
		ksym_hitcount[i] = 0;
		ksymid_sortedlist[i] = i;
	}

	/* calculation total hit for each ksym */
	for (i = 0; i < MAX_SAMPLING_COUNT; i++) {
		symid = ksym_lookup(sampled_pc[i]);
		if (symid < 0)
			continue;
		ksym_hitcount[symid]++;
	}

	/* create a list from highest hit to lowest hit */
	sort(ksymid_sortedlist, ksym_total(),
			sizeof(ksymid_sortedlist[0]), cmp_symhit);

	/* output the result */
	*hitcountp = ksym_hitcount;
	*symid_list = ksymid_sortedlist;
}
