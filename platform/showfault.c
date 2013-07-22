#include <platform/cortex_m.h>
#include <debug.h>

#define CFSR_ADDR	(uint32_t *) (0xE000ED28)
#define MMFSR           (uint8_t) (*CFSR_ADDR & 0xFF)
#define BFSR            (uint8_t) ((*CFSR_ADDR & 0xFF00) >> 8)
#define UFSR            (uint16_t) ((*CFSR_ADDR & 0xFFFF0000) >> 16)

#define MMFAR           (uint32_t *) (0xE000ED34)     /* Memory Management Fault Address Register */
#define BFAR            (uint32_t *) (0xE000ED38)     /* BusFault Address Register */

/* MMFSR */
#define MMFSR_MMARVALID		(1 << 7)  /* MMAR has valid contents */
#define MMFSR_MLSPERR		(1 << 5)  /* MemoryFault on FP lazy state preservation */
#define MMFSR_MSTKERR		(1 << 4)  /* Derived MemoryFault on exception entry */
#define MMFSR_MUSTKERR		(1 << 3)  /* Derived MemoryFault on exception return */
#define MMFSR_DACCVIOL		(1 << 1)  /* Data access violation */
#define	MMFSR_IACCVIOL		(1 << 0)  /* access violation on instruction fetch */ 

/* BFSR */
#define BFSR_BFARVALID 		(1 << 7)  /* BFAR has valid contents */
#define BFSR_LSPERR		(1 << 5)  /* BusFault on FP lazy state preservation */
#define BFSR_STKERR		(1 << 4)  /* Derived BusFault on exception entry */
#define BFSR_MUSTKERR		(1 << 3)  /* Derived BusFault on exception return */
#define BFSR_IMPRECISERR	(1 << 2)  /* Imprecise data access error */
#define BFSR_PRECISERR		(1 << 1)  /* Precise data access error */
#define BFSR_IBUSERR  		(1 << 0)  /* BusFault on instruction prefetch */

/* UFSR */
#define UFSR_DIVBYZERO 		(1 << 9)  /* divide by zero error */
#define UFSR_UNALIGNED 		(1 << 8)  /* unaligned access error */
#define UFSR_NOCP      		(1 << 3)  /* coprocessor access error */
#define UFSR_INVPC     		(1 << 2)  /* integrity check error on EXC_RETURN */
#define UFSR_INVSTATE  		(1 << 1)  /* instruction executed with invalid EPSR.T EPSR.IT */
#define UFSR_UNDEFINSTR		(1 << 0)  /* undefined instruction */

/* Show the status of MPU faults */
void MMFaultStatus()
{
	uint8_t mmfsr = MMFSR;

	dbg_printf(DL_EMERG, "MFSR 0x%02x\n", mmfsr);
	if (mmfsr & MMFSR_MLSPERR)
		dbg_puts("MM Fault on FP lazy state preservation\n");
	if (mmfsr & MMFSR_MSTKERR)
		dbg_puts("MM Fault on exception entry\n");
	if (mmfsr & MMFSR_MUSTKERR)
		dbg_puts("MM Fault on exception return\n");
	if (mmfsr & MMFSR_DACCVIOL)
		dbg_printf(DL_EMERG, "MM Fault on data access violation, 0x%08x\n", *MMFAR);
	if (mmfsr & MMFSR_IACCVIOL)
		dbg_printf(DL_EMERG, "MM Fault on instruction fetch, 0x%08x\n", *MMFAR);
}

void BusFaultStatus()
{
	uint8_t bfsr = BFSR;

	dbg_printf(DL_EMERG, "BFSR 0x%02x\n", bfsr);
	if (bfsr & BFSR_LSPERR)
		dbg_puts("BusFault on FP lazy state preservation\n");
	if (bfsr & BFSR_STKERR)
		dbg_puts("BusFault on exception entry\n");
	if (bfsr & BFSR_MUSTKERR)
		dbg_puts("BusFault on exception return\n");
	if (bfsr & BFSR_IMPRECISERR)
		dbg_puts("BusFault on imprecise data access\n");
	if (bfsr & BFSR_PRECISERR)
		dbg_printf(DL_EMERG, "BusFault on precise data access, 0x%08x\n", *BFAR);
	if (bfsr & BFSR_IBUSERR)
		dbg_puts("BusFault on instructino prefetch\n");
}

void UsageFaultStatus()
{
	uint16_t ufsr = UFSR;

	dbg_printf(DL_EMERG, "UFSR 0x%04x\n", ufsr);
	if (ufsr & UFSR_DIVBYZERO)
		dbg_puts("UsageFault on divide-by-zero\n");
	if (ufsr & UFSR_UNALIGNED)
		dbg_puts("UsageFault on unaligned access error\n");
	if (ufsr & UFSR_NOCP)
		dbg_puts("UsageFault on coprocessor access error\n");
	if (ufsr & UFSR_INVPC)
		dbg_puts("UsageFault on EXC_RETURN integrity check error\n");
	if (ufsr & UFSR_INVSTATE)
		dbg_puts("UsageFault on invalid EPSR.T or EPSR.IT\n");
	if (ufsr & UFSR_UNDEFINSTR)
		dbg_puts("UsageFault on undefined instruction\n");
}

void HardFaultStatus()
{
	uint32_t hfsr = *SCB_HFSR;

	dbg_printf(DL_EMERG, "HFSR 0x%08x\n", hfsr);
	if (hfsr & SCB_HFSR_VECTTBL)
		dbg_puts("HardFault on vector table reading error\n");
	if (hfsr & SCB_HFSR_FORCED)
		dbg_puts("HardFault on priority escalation\n");
	if (hfsr & SCB_HFSR_DEBUGEVT)
		dbg_puts("HardFault on debug event\n");
}

void showfault()
{
	MMFaultStatus();
	BusFaultStatus();
	UsageFaultStatus();
	HardFaultStatus();
}
