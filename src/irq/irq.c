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

#include "irq.h"
#include "mm/mm.h"
#include "arch/ppc440.h"

#include <stdio.h>
#include <string.h>

int irq_init()
{
     U32 msr;
     /*
       Map the memory area that will be used for the exception handlers.
       Each handler must be installed on a 16-byte aligned address.

       We will allocate 255 (0xFF) byte for each handler. This will be plenty
       as each handler will only do some minor work, then call a C-function.
       We install the handlers starting at 0x0.

       We also map up a 1024 byte area, 512 bytes of which will be used as stack.
       The remaining 512 bytes will be used for storing registers.
       This area will be right after IVOR15.
      */

     __mm_map_region(IRQ_IVOR_BASE, IRQ_IVOR_BASE, 0, IRQ_IVOR_SIZE * IRQ_IVOR_COUNT,
		     TLB_PERM_SR|TLB_PERM_SW|TLB_PERM_SX, TLB_ATTR_NONE,
		     __MM_LOCK_TLB|__MM_WRITE_TLB, TLB_SIZE_1KB);
     __mm_map_region(IRQ_STACK_BASE, IRQ_STACK_BASE, 0, IRQ_STACK_SIZE,
		     TLB_PERM_SR|TLB_PERM_SW, TLB_ATTR_NONE,
		     __MM_LOCK_TLB|__MM_WRITE_TLB, TLB_SIZE_1KB);

     /* Install the handlers */
     irq_install_exception_handler(_ivor_critical_int, IRQ_IVOR_CRIT_INPUT);
     irq_install_exception_handler(_ivor_machine_check, IRQ_IVOR_MACH_CHECK);
     irq_install_exception_handler(_ivor_data_storage, IRQ_IVOR_DATA_STORE);
     irq_install_exception_handler(_ivor_instruction_storage, IRQ_IVOR_INST_STORE);
     irq_install_exception_handler(_ivor_external_input, IRQ_IVOR_EXTE_INPUT);
     irq_install_exception_handler(_ivor_alignment, IRQ_IVOR_ALIGNMENT);
     irq_install_exception_handler(_ivor_program, IRQ_IVOR_PROGRAM);
     irq_install_exception_handler(_ivor_fp_unavail, IRQ_IVOR_FP_UNAVAIL);
     irq_install_exception_handler(_ivor_system_call, IRQ_IVOR_SYST_CALL);
     irq_install_exception_handler(_ivor_ap_unavail, IRQ_IVOR_AUX_PROC_UNAVAIL);
     irq_install_exception_handler(_ivor_decrementer, IRQ_IVOR_DECREMENTER);
     irq_install_exception_handler(_ivor_fixed_interval_timer, IRQ_IVOR_FIXED_TIME);
     irq_install_exception_handler(_ivor_watchdog_timer, IRQ_IVOR_WDOG_TIME);
     irq_install_exception_handler(_ivor_data_tlb_error, IRQ_IVOR_DATA_TLB_ERR);
     irq_install_exception_handler(_ivor_instruction_tlb_error, IRQ_IVOR_INST_TLB_ERR);
     irq_install_exception_handler(_ivor_debug, IRQ_IVOR_DEBUG);

     /* Enable critical interrupts and machine check exceptions */
     MFMSR(msr);
     msr |= MSR_CRTICAL_IRQ_ENABLE|MSR_MACHINE_CHECK_ENABLE;
     MTMSR(msr);

     return 0;
}

int irq_install_exception_handler(void(handler(void)), U8 ivor)
{
        U32 ivorAddr;

        if( ivor > 15 )
                return -1;

        memcpy((void *)(IRQ_IVOR_BASE + IRQ_IVOR_SIZE * ivor), handler, IRQ_IVOR_SIZE);
        
        ivorAddr = (IRQ_IVOR_BASE + IRQ_IVOR_SIZE * ivor);
        
        /* How to do this in a good way? We can not pass the SPR number with "=r"... */

        switch(IRQ_IVOR0 + ivor) {
        case IRQ_IVOR0:
                MTSPR(ivorAddr, IRQ_IVOR0);
                break;
        case IRQ_IVOR1:
                MTSPR(ivorAddr, IRQ_IVOR1);
                break;
        case IRQ_IVOR2:
                MTSPR(ivorAddr, IRQ_IVOR2);                
                break;
        case IRQ_IVOR3:
                MTSPR(ivorAddr, IRQ_IVOR3);
                break;
        case IRQ_IVOR4:
                MTSPR(ivorAddr, IRQ_IVOR4);
                break;
        case IRQ_IVOR5:
                MTSPR(ivorAddr, IRQ_IVOR5);
                break;
        case IRQ_IVOR6:
                MTSPR(ivorAddr, IRQ_IVOR6);
                break;
        case IRQ_IVOR7:
                MTSPR(ivorAddr, IRQ_IVOR7);
                break;
        case IRQ_IVOR8:
                MTSPR(ivorAddr, IRQ_IVOR8);
                break;
        case IRQ_IVOR9:
                MTSPR(ivorAddr, IRQ_IVOR9);
                break;
        case IRQ_IVOR10:
                MTSPR(ivorAddr, IRQ_IVOR10);
                break;
        case IRQ_IVOR11:
                MTSPR(ivorAddr, IRQ_IVOR11);
                break;
        case IRQ_IVOR12:
                MTSPR(ivorAddr, IRQ_IVOR12);
                break;
        case IRQ_IVOR13:
                MTSPR(ivorAddr, IRQ_IVOR13);
                break;
        case IRQ_IVOR14:
                MTSPR(ivorAddr, IRQ_IVOR14);
                break;
        case IRQ_IVOR15:
                MTSPR(ivorAddr, IRQ_IVOR15);
                break;
        }

        return 0;
}
