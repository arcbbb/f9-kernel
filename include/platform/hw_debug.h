/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_HW_DEBUG_H_
#define PLATFORM_HW_DEBUG_H_

typedef struct {
	uint32_t comp;
	uint32_t mask;
	uint32_t func;
	uint32_t reserved;
} dwt_comp_t;

/* DWT: Data Watchpoint and Trace unit Registers */
#define DWT_BASE		(uint32_t) (0xE0001000)
#define DWT_CTRL		(volatile uint32_t *) (DWT_BASE)		/*!< Control Register */
#define DWT_CYCCNT		(volatile uint32_t *) (DWT_BASE + 0x04)		/*!< Cycle Count Register */
#define DWT_CPICNT		(volatile uint32_t *) (DWT_BASE + 0x08)		/*!< CPI Count Register */
#define DWT_EXCCNT		(volatile uint32_t *) (DWT_BASE + 0x0c)		/*!< Exception Overhead Count Register */
#define DWT_SLEEPCNT		(volatile uint32_t *) (DWT_BASE + 0x10)		/*!< Sleep Count Register */
#define DWT_LSUCNT		(volatile uint32_t *) (DWT_BASE + 0x14)		/*!< LSU Count Register */
#define DWT_FOLDCNT		(volatile uint32_t *) (DWT_BASE + 0x18)		/*!< Folded-instruction Count Register */
#define DWT_PCSR		(volatile uint32_t *) (DWT_BASE + 0x1c)		/*!< Program Counter Sample Register */
#define DWT_COMP		(volatile dwt_comp_t *) (DWT_BASE + 0x20)	/*!< Comparator Registers */

/* DWT Control Register */
#define DWT_CTRL_CYCCNTENA	(uint32_t) (1 << 0)	/*!< Enable CYCCNT */
#define DWT_CTRL_CPIEVTENA	(uint32_t) (17 << 0)	/*!< Enable CPI count event */
#define DWT_CTRL_EXCEVTENA	(uint32_t) (18 << 0)	/*!< Enable Exception Overhead counter event */
#define DWT_CTRL_SLEEPEVTENA	(uint32_t) (19 << 0)	/*!< Enable Sleep counter event */
#define DWT_CTRL_LSUEVTENA	(uint32_t) (20 << 0)	/*!< Enable LSU counter event */
#define DWT_CTRL_FOLDEVTENA	(uint32_t) (21 << 0)	/*!< Enable Folded-instruction counter event */

/* DWT Comparator Function Register */
#define DWT_FUNC_DISABLE	(uint32_t) (0 << 0)	/*!< Comparator disabled */
#define DWT_FUNC_CYCWATCH	(uint32_t) (4 << 0)	/*!< Watch cycle count */
#define DWT_FUNC_DADDR_RO	(uint32_t) (5 << 0)	/*!< Watch read-only data-access */
#define DWT_FUNC_DADDR_WO	(uint32_t) (6 << 0)	/*!< Watch write-only data-access */
#define DWT_FUNC_DADDR_RW	(uint32_t) (7 << 0)	/*!< Watch read-write data-access */
#define DWT_FUNC_CYCMATCH	(uint32_t) (1 << 7)	/*!< Enable cycle count comparison */
#define DWT_FUNC_DATAVMATCH	(uint32_t) (1 << 8)	/*!< Enable data value comparison */
#define DWT_FUNC_DATASZ_BYTE	(uint32_t) (0 << 10)	/*!< Size of data compared: 00 = Byte */
#define DWT_FUNC_DATASZ_HWORD	(uint32_t) (1 << 10)	/*!< Size of data compared: 01 = Half-Word */
#define DWT_FUNC_DATASZ_WORD	(uint32_t) (2 << 10)	/*!< Size of data compared: 10 = Word */
#define DWT_FUNC_MATCHED	(uint32_t) (1 << 24)	/*!< Operation defined by FUNC matched */

/* FPB Flash Patch and Breakpoint unit Registers */
#define FPB_MAX_COMP		6
#define FPB_BASE		(uint32_t) (0xE0002000)
#define FPB_CTRL		(volatile uint32_t *) (FPB_BASE)
#define FPB_REMAP		(volatile uint32_t *) (FPB_BASE + 0x04)
#define FPB_COMP		(volatile uint32_t *) (FPB_BASE + 0x08)

#define FPB_CTRL_ENABLE		(uint32_t) (1 << 0)
#define FPB_CTRL_KEY		(uint32_t) (1 << 1)

#define FPB_COMP_ENABLE		(uint32_t) (1 << 0)
#define FPB_COMP_REPLACE_LOWER	(uint32_t) (1 << 30)
#define FPB_COMP_REPLACE_UPPER	(uint32_t) (2 << 30)

void watchpoint_handler(uint32_t *stack);
void watch_cycle_countdown(uint32_t cycle, int (*func)(uint32_t *));

void hw_debug_init();
int breakpoint_install(uint32_t addr);
void breakpoint_uninstall(int id);

#define cpu_enable_single_step()				\
	do {							\
		*DCB_DEMCR |= DCB_DEMCR_MON_STEP;		\
	} while (0)
#define cpu_disable_single_step()				\
	do {							\
		*DCB_DEMCR &= ~DCB_DEMCR_MON_STEP;		\
	} while (0)

#define enable_bkptid(id)					\
	do {							\
		*(FPB_COMP + (id)) |= FPB_COMP_ENABLE;		\
	} while (0)
#define disable_bkptid(id)					\
	do {							\
		*(FPB_COMP + (id)) &= ~FPB_COMP_ENABLE;		\
	} while (0)

#define enable_hw_breakpoint()					\
	do {							\
		*FPB_CTRL = FPB_CTRL_KEY | FPB_CTRL_ENABLE;	\
	} while (0)
#define disable_hw_breakpoint()					\
	do {							\
		*FPB_CTRL = FPB_CTRL_KEY | ~FPB_CTRL_ENABLE;	\
	} while (0)

#define DWT_COMP0_CYCLE(cycle)					\
	/* enable CMP[0] */					\
	(*(DWT_COMP + 0)).comp = 0xFFFFFFFF;			\
	(*(DWT_COMP + 0)).mask = 0;				\
	(*(DWT_COMP + 0)).func = DWT_FUNC_CYCMATCH | DWT_FUNC_CYCWATCH;\
	/* enable CYCCNT */					\
	*DWT_CYCCNT = 0xFFFFFFFF - cycle;			\
	*DWT_CTRL |= DWT_CTRL_CYCCNTENA;

#endif /* PLATFORM_HW_DEBUG_H_ */
