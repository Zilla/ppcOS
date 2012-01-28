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

#include <stdio.h>
#include "mm/mm.h"
#include "log/log.h"
#include "arch/ppc440.h"
#include "timer/timer.h"
#include "sched/scheduler.h"

extern U32 pid;

void backupRegs()
{
     /* Save all regs. r1 is saved before we enter here */
     /* This mess should be re-written */
     asm volatile (
/*	"stw	0,0x1080(0);" */
	"stw	2,0x1008(0);"
	"stw	3,0x100C(0);"
	"stw	4,0x1010(0);"
	"stw	5,0x1014(0);"
	"stw	6,0x1018(0);"
	"stw	7,0x101C(0);"
	"stw	8,0x1020(0);"
	"stw	9,0x1024(0);"
	"stw	10,0x1028(0);"
	"stw	11,0x102C(0);"
	"stw	12,0x1030(0);"
	"stw	13,0x1034(0);"
	"stw	14,0x1038(0);"
	"stw	15,0x103C(0);"
	"stw	16,0x1040(0);"
	"stw	17,0x1044(0);"
	"stw	18,0x1048(0);"
	"stw	19,0x104C(0);"
	"stw	20,0x1050(0);"
	"stw	21,0x1054(0);"
	"stw	22,0x1058(0);"
	"stw	23,0x105C(0);"
	"stw	24,0x1060(0);"
	"stw	25,0x1064(0);"
	"stw	26,0x1068(0);"
	"stw	27,0x106C(0);"
	"stw	28,0x1070(0);"
	"stw	29,0x1074(0);"
	"stw	30,0x1078(0);"
/*	"stw	31,0x107C(0)" */
	  );
}

void restoreRegs()
{
     /* Restore all regs. r1 will be restored later */
     asm volatile (
/* 	"lwz	0,0x1080(0);" */
	"lwz	2,0x1008(0);"
	"lwz	3,0x100C(0);"
	"lwz	4,0x1010(0);"
	"lwz	5,0x1014(0);"
	"lwz	6,0x1018(0);"
	"lwz	7,0x101C(0);"
	"lwz	8,0x1020(0);"
	"lwz	9,0x1024(0);"
	"lwz	10,0x1028(0);"
	"lwz	11,0x102C(0);"
	"lwz	12,0x1030(0);"
	"lwz	13,0x1034(0);"
	"lwz	14,0x1038(0);"
	"lwz	15,0x103C(0);"
	"lwz	16,0x1040(0);"
	"lwz	17,0x1044(0);"
	"lwz	18,0x1048(0);"
	"lwz	19,0x104C(0);"
	"lwz	20,0x1050(0);"
	"lwz	21,0x1054(0);"
	"lwz	22,0x1058(0);"
	"lwz	23,0x105C(0);"
	"lwz	24,0x1060(0);"
	"lwz	25,0x1064(0);"
	"lwz	26,0x1068(0);"
	"lwz	27,0x106C(0);"
	"lwz	28,0x1070(0);"
	"lwz	29,0x1074(0);"
	"lwz	30,0x1078(0);"
/*	"lwz	31,0x107C(0)" */
	  );
}

void ivor_critical_int ()
{
     backupRegs();

     printf("Critical Interrupt!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_machine_check ()
{
     backupRegs();

     printf("Machine Check!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_data_storage ()
{
     backupRegs();

     printf("Data storage!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_instruction_storage ()
{
     backupRegs();

     printf("Instruction storage!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_external_input ()
{
     backupRegs();

     printf("External input!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_alignment ()
{
     backupRegs();

     printf("Alignment!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_program ()
{
     backupRegs();

     printf("Program!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_fp_unavail ()
{
     backupRegs();

     printf("FP Unavailable!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_system_call ()
{
     backupRegs();

     printf("System Call!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_ap_unavail ()
{
     backupRegs();

     printf("AP Unavailable!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_decrementer ()
{
     backupRegs();

     printf("Decrementer!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_fixed_interval_timer ()
{
     U32 tsr, srr1;

     backupRegs();

     add_tick();
     sched_invoke(true);

     MFSPR(tsr, TSR);
     tsr |= TSR_FIS;
     MTSPR(tsr, TSR);

     MFSRR1(srr1);
     srr1 &= ~MSR_WAIT_STATE_ENABLE;
     MTSRR1(srr1);     

     restoreRegs();
}

void ivor_watchdog_timer ()
{
     backupRegs();

     printf("Watchdog Timer!!!\n");
     while(1)
	  ;

     restoreRegs();
}

/* Note: No printf() or write() here, as we can not be sure UART memory is accessible */

void ivor_data_tlb_error()
{
     U32 dear;

     backupRegs();

     MFDEAR(dear);

     if(mm_load_tlb_entry(dear) == EERROR)
     {
	  /* Call error handler here */
	  while(1)
	       ;
     }
     
     restoreRegs();
}

void ivor_instruction_tlb_error ()
{
     backupRegs();

     printf("Instruction TLB error!!!\n");
     while(1)
	  ;

     restoreRegs();
}

void ivor_debug ()
{
     backupRegs();

     printf("Debug!!!\n");
     while(1)
	  ;

     restoreRegs();
}
