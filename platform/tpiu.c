#include <debug.h>
#include <platform/cortex_m.h>
#include <platform/stm32f4/gpio.h>
#include <platform/tpiu.h>

void init_tpiu()
{
	// must set DEMCR.TRCENA before read/write TPIU register
	*DCB_DEMCR |= DCB_DEMCR_TRCENA;

	// Current Parallel Port Size = 0x1
	*TPIU_CSPSR = 0x1;

	// baud rate for SWO
	//SWO output clock = Asynchronous_Reference_Clock/(SWOSCALAR +1)
	//*TPIU_ACPR = 7;
	*TPIU_ACPR = 8;

	// check TPIU Formatter and Flush Control Register == 0x102
	*TPIU_FFCR = TPIU_FFCR_TRIGIN | TPIU_FFCR_ENFCONT;
	//*TPIU_FFCR &= ~TPIU_FFCR_ENFCONT;

	// TPIU Select Pin Protocol 0x2 async NRZ mode
	*TPIU_SPPR = TPIU_SPPR_ASYNC_SWO_NZR;
	//*TPIU_SPPR = TPIU_SPPR_ASYNC_SWO_MANCHESTER;

	// DEBUGMCU CR IO_TRACEN (0x20), TPIU Sync packet FF_FF_FF_7F
	*DBGMCU_CR = DBGMCU_CR_TRACE_IOEN | DBGMCU_CR_TRACE_MODE_ASYNC;

	// Configure ITM

	// ITM Lock Access Register
	*ITM_LAR = 0xC5ACCE55;    /* unlock ITM register */
	/* Enable ITM with ID = 1 */
	//*ITM_TCR = (1 << 16) | ITM_TCR_SYNCENA | ITM_TCR_ITMENA;
	*ITM_TCR = (1 << 16) | ITM_TCR_ITMENA;
	/* Enable stimulus port 1 */
	*(ITM_TERx + 0) = 1;

}

void LED_gpio_toggle(uint8_t port, uint8_t pin)
{
	*GPIO_ODR(port) ^= (1 << pin);
}

void itm_send(char c)
{
	while(!(*(ITM_STIMx + 0) & ITM_STIM_FIFOREADY));
	*(ITM_STIMx + 0) = c;
}

void init_LED_gpio()
{
	int pin;
	// LED3,4,5,6
	// PD13,12,14,15
	for (pin = 12; pin <= 15; pin++) {
		gpio_config_output(GPIOD, pin, GPIO_PUPDR_NONE, 0);
		//gpio_out_high(GPIOD, pin);
	}
}

void kdb_test_tpiu()
{
	static int init = 0;
	int i;
	int ch = 0;
	if (init == 0) {
		init_tpiu();
		init_LED_gpio();
		init++;
	}

	dbg_printf(DL_KDB, "DHCSR : 0x%08x\n", *DCB_DHCSR);
	dbg_printf(DL_KDB, "DEMCR: 0x%08x\n", *DCB_DEMCR);
	dbg_printf(DL_KDB, "DEBUGMCU_CR: 0x%08x\n", *DBGMCU_CR);
	if (*TPIU_TYPE & TPIU_TYPE_NRZVALID) {
		dbg_puts("TPIU support SWO using NRZ encoding\n");
	}
	if (*TPIU_TYPE & TPIU_TYPE_MACVALID) {
		dbg_puts("TPIU support SWO using Manchester encoding\n");
	}
	if (!(*TPIU_TYPE & TPIU_TYPE_PTINVALID)) {
		dbg_puts("TPIU support Parallel trace port\n");
	}
	dbg_printf(DL_KDB, "TPIU FIFO Size: %d\n", (*TPIU_TYPE & TPIU_TYPE_FIFOSZ_MASK) >> 6);
	dbg_printf(DL_KDB, "TPIU Support Parallel Port Size: 0x%08x\n", *TPIU_SSPSR);
	dbg_printf(DL_KDB, "TPIU Current Parallel Port Size: 0x%08x\n", *TPIU_CSPSR);
	dbg_printf(DL_KDB, "TPIU Formatter and Flush Control : 0x%08x  == 0x102\n", *TPIU_FFCR);
	dbg_printf(DL_KDB, "TPIU Select Pin Protocol: 0x%08x  == 0x2\n", *TPIU_SPPR);

	// write ITM Stimulus register
	while (1) {
		LED_gpio_toggle(GPIOD, 13);  // toggle LED
		itm_send(ch + '0');
		ch = (ch == 9) ? 0 : ch + 1;
		if (ch == 9)
			itm_send('\n');
		for (i = 0; i < 800000; i++)
			__asm__("NOP");
	}
}
