/* Copyright (c) 2012, Joakim Östlund
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

#include "scheduler.h"
#include "proc/process.h"
#include "log/log.h"
#include "arch/ppc440.h"

#include <string.h>

void sched_invoke(bool isInterrupt)
{
     bool replaceRunning = false;
     U8 currPri = PRIO_MAX, checkPri;
     Process *pNew = NULL;
     U32 msr;
     U32 *pReg = (U32 *)0x1000; /* TODO: Add define */

     SYNC;
     ISYNC;

     /* TODO: Flush cache */

     if( isInterrupt == false)
     {
	  /* Disable timer interrupt */
	  MFMSR(msr);
	  WRTEEI(0);
	  ISYNC;
     }

     if( procWaitList != NULL && isInterrupt == true )
     {
	  Process *pCurr, *pPrev = NULL;

	  /* Check if sleeping processes should go to ready */
	  /* TODO: We should accurately convert system ticks to ms */
	  /* For now, hardcode 1 tick = 1 ms */
	  pCurr = procWaitList;
	  while( pCurr != NULL )
	  {
	       pCurr->msSleep -= 1;
	       if( pCurr->msSleep <= 0 )
	       {
		    /* Move process to ready list for its prio */
		    if( procReadyList[pCurr->prio] == NULL )
			 procReadyList[pCurr->prio] = pCurr;
		    else
		    {
			 /* At least one other process is ready, put at end of list */
			 Process *pLink = procReadyList[pCurr->prio];

			 while(pLink->pReadyNext != NULL)
			      pLink = pLink->pReadyNext;
			 
			 pLink->pReadyNext = pCurr;
		    }

		    pCurr->state = PROC_READY;

		    /* Remove from wait list */
		    if( pPrev != NULL )
			 pPrev->pWaitNext = pCurr->pWaitNext;
		    else
			 procWaitList = pCurr->pWaitNext;
	       }

	       pPrev = pCurr;
	       pCurr = pCurr->pWaitNext;
	  }
     }

     if( procRunning == NULL )
     {
	  checkPri = PRIO_IDLE + 1;
	  replaceRunning = true;
     }
     else
	  checkPri = procRunning->prio;

     /* Check if there is a higher prio process ready */
     /* We never replace a process of the same prio that is running */
     
     for( currPri = PRIO_MAX; currPri < checkPri; currPri++ )
     {
	  if( procReadyList[currPri] != NULL )
	  {
	       /* Make active process */
	       pNew = procReadyList[currPri];
	       procReadyList[currPri] = procReadyList[currPri]->pReadyNext;
	       replaceRunning = true;

	       break;
	  }
     }

     if( pNew == NULL && replaceRunning == true )
     {
	  ERROR("Failed to find a process to run!");
	  while(1);
     }

     if( replaceRunning == true )
     {
	  if( procRunning != NULL )
	  {
	       /* Store registers of currently running process */
	       procRunning->pcb.regs[PROC_REG_LR]        = *(pReg++);
	       procRunning->pcb.regs[PROC_REG_STACK_PTR] = *(pReg++);

	       memcpy(&(procRunning->pcb.regs[PROC_REG_STACK_PTR + 1]), (void *)pReg,
		      sizeof(U32)*((PROC_REG_GPR_END - PROC_REG_STACK_PTR) - 1));

	       pReg += ((PROC_REG_GPR_END - PROC_REG_STACK_PTR) - 1);

	       /* TODO: Save CR, XER, CTR */

	       /* Save PC */
	       MFSRR0(procRunning->pcb.regs[PROC_REG_PC]);
	  }

	  if( procRunning != NULL && procRunning->state == PROC_RUNNING )
	  {
	       /*
		 If the process is exempted while running, put it
		 at the front of the ready queue for that priority.
		 This will allow it to continue executing after all
		 higer prio processes are done.
	       */
	       
	       procRunning->pReadyNext = procReadyList[procRunning->prio];
	       procReadyList[procRunning->prio] = procRunning;

	       procRunning->state = PROC_READY;
	  }

	  procRunning = pNew;
	  pReg        = (U32 *)0x1000;

	  /* Restore registers for the new process */
	  *(pReg++) = procRunning->pcb.regs[PROC_REG_LR];
	  *(pReg++) = procRunning->pcb.regs[PROC_REG_STACK_PTR];

	  memcpy((void *)pReg, &(procRunning->pcb.regs[PROC_REG_STACK_PTR + 1]),
		      sizeof(U32)*((PROC_REG_GPR_END - PROC_REG_STACK_PTR) - 1));

	  pReg += ((PROC_REG_GPR_END - PROC_REG_STACK_PTR) - 1);

	  /* TODO: Restore CR, XER, CTR */

	  /* Restore PC */
	  MTSRR0(procRunning->pcb.regs[PROC_REG_PC]);
     }

     if( isInterrupt == false)
     {
	  /* Restore MSR */
	  MTMSR(msr);
     }

     SYNC;
     ISYNC;

     /* TODO: Flush cache */

}

/* Implemented in exception_handlers.c */
void backupRegs();
void restoreRegs();

void yield()
{
     backupRegs();
     sched_invoke(false);
     restoreRegs();
}
