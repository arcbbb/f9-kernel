/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <types.h>
#include <platform/gdb_uart.h>

/************************************************************************/
/* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 2048

static const char hexchars[]="0123456789abcdef";

/* Number of registers.  */
#define NUMREGS	16

/* Number of bytes of registers.  */
#define NUMREGBYTES (NUMREGS * 4)

static char *strcpy(char *dest, const char *src)
{
	int i;
	for (i = 0; src[i] != '\0'; i++) {
		dest[i] = src[i];
	}
	return dest;
}

#define putDebugChar(ch) (gdb_putchar(ch))

#define getDebugChar() (gdb_getchar())

/* Convert ch from a hex digit to an int */

static int hex(unsigned char ch)
{
	if (ch >= 'a' && ch <= 'f')
		return ch-'a'+10;
	if (ch >= '0' && ch <= '9')
		return ch-'0';
	if (ch >= 'A' && ch <= 'F')
		return ch-'A'+10;
	return -1;
}

static unsigned char remcomInBuffer[BUFMAX];
static unsigned char remcomOutBuffer[BUFMAX];

/* scan for the sequence $<data>#<checksum> */

static unsigned char * getpacket(void)
{
	unsigned char *buffer = &remcomInBuffer[0];
	unsigned char checksum;
	unsigned char xmitcsum;
	int count;
	char ch;

	while (1) {
		/* wait around for the start character,
		 * ignore all other characters.
		 */
		while ((ch = getDebugChar ()) != '$');

retry:
		checksum = 0;
		xmitcsum = -1;
		count = 0;

		/* now, read until a # or end of buffer is found */
		while (count < BUFMAX - 1) {
			ch = getDebugChar ();
			if (ch == '$')
				goto retry;
			if (ch == '#')
				break;
			checksum = checksum + ch;
			buffer[count] = ch;
			count = count + 1;
		}
		buffer[count] = 0;

		if (ch == '#') {
			ch = getDebugChar ();
			xmitcsum = hex (ch) << 4;
			ch = getDebugChar ();
			xmitcsum += hex (ch);

			if (checksum != xmitcsum) {
				putDebugChar ('-');	/* failed checksum */
			}
			else {
				putDebugChar ('+');	/* successful transfer */

				/* if a sequence char is present,
				 * reply the sequence ID
				 */
				if (buffer[2] == ':') {
					putDebugChar (buffer[0]);
					putDebugChar (buffer[1]);

					return &buffer[3];
				}

				return &buffer[0];
			}
		}
	}
}

/* send the packet in buffer.  */

static void putpacket(unsigned char *buffer)
{
	unsigned char checksum;
	int count;
	unsigned char ch;

	/*  $<packet info>#<checksum>. */
	do {
		putDebugChar('$');
		checksum = 0;
		count = 0;

		while ((ch = buffer[count])) {
			putDebugChar(ch);
			checksum += ch;
			count += 1;
		}

		putDebugChar('#');
		putDebugChar(hexchars[checksum >> 4]);
		putDebugChar(hexchars[checksum & 0xf]);

	}
	while (getDebugChar() != '+');
}

/* Indicate to caller of mem2hex or hex2mem that there has been an
   error.  */
static volatile int mem_err = 0;

/* Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf (null), in case of mem fault,
 * return 0.
 * If MAY_FAULT is non-zero, then we will handle memory faults by returning
 * a 0, else treat a fault like any other fault in the stub.
 */

static unsigned char * mem2hex (unsigned char *mem, unsigned char *buf,
		int count, int may_fault)
{
	unsigned char ch;

	// TODO
	// set_mem_fault_trap(may_fault);

	while (count-- > 0) {
		ch = *mem++;
		if (mem_err)
			return 0;
		*buf++ = hexchars[ch >> 4];
		*buf++ = hexchars[ch & 0xf];
	}

	*buf = 0;

	// TODO
	// set_mem_fault_trap(0);

	return buf;
}

/* convert the hex array pointed to by buf into binary to be placed in mem
 * return a pointer to the character AFTER the last byte written */

static unsigned char * hex2mem (unsigned char *buf, unsigned char *mem, int count,
		int may_fault)
{
	int i;
	unsigned char ch;

	// TODO
	//set_mem_fault_trap(may_fault);

	for (i=0; i<count; i++)
	{
		ch = hex(*buf++) << 4;
		ch |= hex(*buf++);
		*mem++ = ch;
		if (mem_err)
			return 0;
	}

	// TODO
	//set_mem_fault_trap(0);

	return mem;
}

/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */

static int hexToInt(unsigned char **ptr, int *intValue)
{
	int numChars = 0;
	int hexValue;

	*intValue = 0;

	while (**ptr) {
		hexValue = hex(**ptr);
		if (hexValue < 0)
			break;

		*intValue = (*intValue << 4) | hexValue;
		numChars ++;

		(*ptr)++;
	}

	return (numChars);
}

/*
 * target remote /dev/ttyUSB0
 * Sending packet: $qSupported:qRelocInsn+#9a...
 * add-symbol-file 
 * Sending packet: $qSymbol::#5b...putpkt: Junk:
 * list
 * Sending packet: $m80099a8,4#70...putpkt: Junk:
 * info register
 * msp, psp, control, faultmask, basepri, primask
 * Sending packet: $p1a#02...putpkt: Junk: msp
 * Packet received: 00040020
 * 0x20000400	0x20000400
 */

void gdb_handle_exception(uint32_t *registers)
{
	int sigval;
	int addr;
	int length;
	unsigned char *ptr;
	//unsigned long *sp;

	/* reply to host that an exception has occurred */
	//putpacket(remcomOutBuffer);
	sigval = 0;
	while (1) {
		remcomOutBuffer[0] = 0;

		ptr = getpacket();
		switch (*ptr++) {
			case 'q':
				remcomOutBuffer[0] = '\0';
				break;
			case '?':
				remcomOutBuffer[0] = 'S';
				remcomOutBuffer[1] = hexchars[sigval >> 4];
				remcomOutBuffer[2] = hexchars[sigval & 0xf];
				remcomOutBuffer[3] = 0;
				break;
			case 'd':		/* toggle debug flag */
				break;
			case 'g':		/* return the value of the CPU registers */
				mem2hex ((void *) registers, remcomOutBuffer, NUMREGBYTES, 0);
				break;
			case 'G':		/* set the value of the CPU registers - return OK */
				hex2mem (ptr, (void *) registers, NUMREGBYTES, 0);
				strcpy ((char *)remcomOutBuffer, "OK");
				break;
			case 'P':		/* set the value of a single CPU register - return OK */
				{
					int regno;

					if (hexToInt (&ptr, &regno) && *ptr++ == '=')
						if (regno >= 0 && regno < NUMREGS) {
							hex2mem (ptr, (void *) &registers[regno], 4, 0);
							strcpy ((char *)remcomOutBuffer, "OK");
							break;
						}

					strcpy ((char *)remcomOutBuffer, "E01");
					break;
				}
			case 'm':	  /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
				/* Try to read %x,%x.  */

				if (hexToInt(&ptr, &addr)
						&& *ptr++ == ','
						&& hexToInt(&ptr, &length)) {
					if (mem2hex((void *)addr, remcomOutBuffer, length, 1))
						break;

					strcpy ((char *)remcomOutBuffer, "E03");
				}
				else {
					strcpy((char *)remcomOutBuffer,"E01");
				}
				break;
			case 'c':    /* cAA..AA    Continue at address AA..AA(optional) */
				/* try to read optional parameter, pc unchanged if no parm */
				return;
				break;
				/*
			case 's':
				// set single-step

				reply = strdup("S05"); // TRAP
				break;
				*/
		}
		/* reply to the request */
		putpacket(remcomOutBuffer);
	}
}
