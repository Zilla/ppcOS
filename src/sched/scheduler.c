/* Copyright (c) 2014, Joakim Östlund
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
#include "linkutil.h"

#include <string.h>

void sched_invoke(bool isInterrupt)
{
     bool replaceRunning = false;
     U8 currPri = PRIO_MAX, checkPri;
     __Process *pNew = NULL;
     U32 msr;

     /* TODO: A lot of stuff here is hardcoded. Refactor code and clean up! */

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

     if( __procWaitList != NULL && isInterrupt == true )
     {
	  __Process *pCurr, *pPrev = NULL;
	  
	  /* Check if sleeping processes should go to ready */
	  /* TODO: We should accurately convert system ticks to ms */
	  /* For now, hardcode 1 tick = 1 ms */
	  pCurr = __procWaitList;
	  while( pCurr != NULL )
	  {
	       pCurr->msSleep -= 1;
	       if( pCurr->msSleep <= 0 )
	       {
		    /* Move process to ready list for its prio */
		    if( __procReadyList[pCurr->prio] == NULL )
			 __procReadyList[pCurr->prio] = pCurr;
		    else
		    {
			 /* At least one other process is ready, put at end of list */
			 LINK_AT_END_EX(__Process, pCurr, __procReadyList[pCurr->prio], pReadyNext);
		    }
		    
		    pCurr->state = __PROC_READY;
		    pCurr->pReadyNext = NULL;
		    
		    /* Remove from wait list */
		    if( pPrev != NULL )
			 pPrev->pWaitNext = pCurr->pWaitNext;
		    else
			 __procWaitList = pCurr->pWaitNext;

		    pCurr->pWaitNext = NULL;
	       }
	       
	       pPrev = pCurr;
	       pCurr = pCurr->pWaitNext;
	  }
     }
     
     if( __procRunning == NULL || __procRunning->state != __PROC_RUNNING )
     {
	  /* Nothing is running, so take anything */
	  checkPri = __PRIO_IDLE + 1;
	  replaceRunning = true;
     }
     else
	  checkPri = __procRunning->prio;

     /* Check if there is a higher prio process ready */
     /* We never replace a process of the same prio that is running */
     
     for( currPri = PRIO_MAX; currPri < checkPri; currPri++ )
     {
	  if( __procReadyList[currPri] != NULL )
	  {
	       /* Make active process */
	       pNew = __procReadyList[currPri];
	       __procReadyList[currPri] = __procReadyList[currPri]->pReadyNext;
	       replaceRunning = true;
	       pNew->pReadyNext = NULL;

	       break;
	  }
     }

     if( pNew == NULL && replaceRunning == true )
     {
	  ERROR("Failed to find a process to run!");
	  pNew = __procList;
	  /* Dump error information */
	  while(pNew)
	  {
	       INFO(STR("pNew: 0x%x, pNew->pNext: 0x%x", pNew, pNew->pNext));
	       INFO(STR("State: %u", pNew->state));
	       INFO(STR("msSleep: %u", pNew->msSleep));
	       INFO(STR("PC: 0x%x", pNew->pcb.regs[__PROC_REG_PC]));
	       INFO(pNew->pcb.procName);
	       pNew = pNew->pNext;
	  }
	  INFO(STR("Running: 0x%x", (U32)__procRunning));
	  while(1);
     }

     if( replaceRunning == true )
     {
	  if( __procRunning != NULL )
	  {
	       /* Store registers of currently running process */
	       __procRunning->pcb.regs[__PROC_REG_LR]        = *((U32 *)0x1000);
	       __procRunning->pcb.regs[__PROC_REG_STACK_PTR] = *((U32 *)0x1004);
	       __procRunning->pcb.regs[__PROC_REG_GPR_START] = *((U32 *)0x1080);

	       memcpy(&(__procRunning->pcb.regs[2]), ((U32 *)0x1008), sizeof(U32)*30);

	       /* Save CR, XER, CTR */
	       __procRunning->pcb.regs[__PROC_REG_CR]  = *((U32 *)0x1084);
	       __procRunning->pcb.regs[__PROC_REG_CTR] = *((U32 *)0x1088);
	       __procRunning->pcb.regs[__PROC_REG_XER] = *((U32 *)0x108C);

	       /* Save PC */
	       MFSRR0(__procRunning->pcb.regs[__PROC_REG_PC]);
	  }

	  if( __procRunning != NULL && __procRunning->state == __PROC_RUNNING )
	  {
	       /*
		 If the process is preempted while running, put it
		 at the front of the ready queue for that priority.
		 This will allow it to continue executing after all
		 higer prio processes are done.
	       */
       	       __procRunning->pReadyNext = __procReadyList[__procRunning->prio];
	       __procReadyList[__procRunning->prio] = __procRunning;

	       __procRunning->state = __PROC_READY;
	  }

	  __procRunning = pNew;

	  /* Restore registers for the new process */
	  *((U32 *)0x1000) = __procRunning->pcb.regs[__PROC_REG_LR];
	  *((U32 *)0x1004) = __procRunning->pcb.regs[__PROC_REG_STACK_PTR];
	  *((U32 *)0x1080) = __procRunning->pcb.regs[__PROC_REG_GPR_START];

	  memcpy(((U32 *)0x1008), &(__procRunning->pcb.regs[2]), sizeof(U32)*30);

	  /* Restore CR, XER, CTR */
	  *((U32 *)0x1084) = __procRunning->pcb.regs[__PROC_REG_CR];
	  *((U32 *)0x1088) = __procRunning->pcb.regs[__PROC_REG_CTR];
	  *((U32 *)0x108C) = __procRunning->pcb.regs[__PROC_REG_XER];

	  /* Restore PC */
	  MTSRR0(__procRunning->pcb.regs[__PROC_REG_PC]);

	  __procRunning->state = __PROC_RUNNING;
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
