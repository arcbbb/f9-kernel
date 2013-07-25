/* Copyright (c) 2013 The F9 Microkernel Project. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PLATFORM_TPIU_H_
#define PLATFORM_TPIU_H_

/* TPIU Register */
#define TPIU_SSPSR                      (volatile uint32_t *) (0xE0040000)   /* Supported Parallel Port Size Register */
#define TPIU_CSPSR                      (volatile uint32_t *) (0xE0040004)   /* Current Parallel Port Size Register */
#define TPIU_ACPR                       (volatile uint32_t *) (0xE0040010)   /* Asynchronous Clock Prescaler Register */
#define TPIU_SPPR                       (volatile uint32_t *) (0xE00400F0)   /* Selected Pin Protocol */
#define TPIU_TYPE                       (volatile uint32_t *) (0xE0040FC8)   /* TPIU Type Register */

#define TPIU_FFSR                       (volatile uint32_t *) (0xE0040300)   /* Formatter and Flush Status */
#define TPIU_FFCR                       (volatile uint32_t *) (0xE0040304)   /* Formatter and Flush Control */

/* Selected Pin Protocol Register */
#define TPIU_SPPR_PARALLEL_TRACE_PORT   (uint32_t) (0x0)                     /* Parallel Trace Port */
#define TPIU_SPPR_ASYNC_SWO_MANCHESTER  (uint32_t) (0x1)                     /* medium-speed asynchronous port */
#define TPIU_SPPR_ASYNC_SWO_NZR         (uint32_t) (0x2)                     /* Low-speed asynchronous port */

/* TPIU Type Register */
#define TPIU_TYPE_FIFOSZ_MASK           (uint32_t) (7 << 6)                  /* Size for TPIU output FIFO */
#define TPIU_TYPE_PTINVALID             (uint32_t) (1 << 9)                  /* No Parallel trace port operation */
#define TPIU_TYPE_MACVALID              (uint32_t) (1 << 10)                 /* SWO using Manchester encoding */
#define TPIU_TYPE_NRZVALID              (uint32_t) (1 << 11)                 /* SWO using NRZ encoding */

/* Formatter and Flush Control Register */
#define TPIU_FFCR_ENFCONT               (uint32_t) (1 << 1)                  /* Activate Formatter */
#define TPIU_FFCR_TRIGIN                (uint32_t) (1 << 8)                  /* Triggers are indicated */

/* DBG Register Map */
#define DBG_BASE                        (uint32_t) (0xE0042000)
#define DBGMCU_IDCODE                   (volatile uint32_t *) (DBG_BASE)
#define DBGMCU_CR                       (volatile uint32_t *) (DBG_BASE + 0x4)

/* DBG Control Register */
#define DBGMCU_CR_TRACE_IOEN            (uint32_t) (1 << 5)
#define DBGMCU_CR_TRACE_MODE_ASYNC      (uint32_t) (0)

/* ITM Register Map */
#define ITM_STIMx                       (volatile uint32_t *) (0xE0000000)         /* Stimulus Port Registers */
#define ITM_TERx                        (volatile uint32_t *) (0xE0000E00)         /* Trace Enable Registers */
#define ITM_TPR                         (volatile uint32_t *) (0xE0000E40)         /* Trace Privilege Register */
#define ITM_TCR                         (volatile uint32_t *) (0xE0000E80)         /* Trace Control Register */
#define ITM_LAR                         (volatile uint32_t *) (0xE0000FB0)         /* Lock Access Register */
#define ITM_LSR                         (volatile uint32_t *) (0xE0000FB4)         /* Lock Status Register */

/* ITM Stimulus Port Register */
#define ITM_STIM_FIFOREADY              (uint32_t) (1 << 0)

/* ITM Trace Control Register */
#define ITM_TCR_ITMENA                  (uint32_t) (1 << 0)                        /* Enable the ITM */
#define ITM_TCR_TSENA                   (uint32_t) (1 << 1)                        /* Enable Local timestamp generation */
#define ITM_TCR_SYNCENA                 (uint32_t) (1 << 2)                        /* Enable Synchronous packet transmission generation */
#define ITM_TCR_DWTENA                  (uint32_t) (1 << 3)                        /* Enable the DWT Stimulus */
#define ITM_TCR_SWOENA                  (uint32_t) (1 << 4)                        /* Enable SWV behavior */

#endif /* PLATFORM_TPIU_H_ */
