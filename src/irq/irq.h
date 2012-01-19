/* Copyright (c) 2011, Joakim Östlund
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ppcos_irq_h_
#define _ppcos_irq_h_

#include "krntypes.h"

#define IRQ_IVOR_BASE  0x0
#define IRQ_IVOR_SIZE  0x100
#define IRQ_IVOR_COUNT 0x10
#define IRQ_STACK_BASE 0x1000
#define IRQ_STACK_SIZE 0x400

/* IVOR definitions */
#define IRQ_IVOR_DTLB_ERR 13    /* Data TLB error (miss) */

/* Exception handler prototypes */
extern void _ivor_critical_int(void);
extern void _ivor_machine_check(void);
extern void _ivor_data_storage(void);
extern void _ivor_instruction_storage(void);
extern void _ivor_external_input(void);
extern void _ivor_alignment(void);
extern void _ivor_program(void);
extern void _ivor_fp_unavail(void);
extern void _ivor_system_call(void);
extern void _ivor_ap_unavail(void);
extern void _ivor_decrementer(void);
extern void _ivor_fixed_interval_timer(void);
extern void _ivor_watchdog_timer(void);
extern void _ivor_data_tlb_error(void);
extern void _ivor_instruction_tlb_error(void);
extern void _ivor_debug(void);


/* SPRs for writing IVOR */
#define IRQ_IVOR0     0x190
#define IRQ_IVOR1     0x191
#define IRQ_IVOR2     0x192
#define IRQ_IVOR3     0x193
#define IRQ_IVOR4     0x194
#define IRQ_IVOR5     0x195
#define IRQ_IVOR6     0x196
#define IRQ_IVOR7     0x197
#define IRQ_IVOR8     0x198
#define IRQ_IVOR9     0x199
#define IRQ_IVOR10    0x19A
#define IRQ_IVOR11    0x19B
#define IRQ_IVOR12    0x19C
#define IRQ_IVOR13    0x19D
#define IRQ_IVOR14    0x19E
#define IRQ_IVOR15    0x19F


int irq_init();
int irq_install_exception_handler(void(handler(void)), U8 ivor);

#endif /* _ppcos_irq_h_ */
