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

#include "process.h"
#include "log/log.h"

#include <string.h>
#include <stdlib.h>
#include <sys/errno.h>

Process *procList        = NULL;
Process *procWaitList    = NULL;
/*Process *procStopList    = NULL;*/
Process *procReadyList[NUM_PRIO];
Process *procRunning     = NULL;

U32      currPid         = 0;

void proc_init()
{
     memset(procReadyList, 0, sizeof(Process *) * NUM_PRIO);

     INFO("Process handling initilized");
}

U32 create_process(char *procName, OSPROC entryPoint, U8 prio, U16 stackSize)
{
     Process *pProc;

     /* Check valid variables. Stack size must be a multiple of 4 */
     if( procName == NULL || entryPoint == NULL || (prio > PRIO_MAX && entryPoint != krnIdle) || (stackSize % 4) != 0 )
     {
	  errno = EINVAL;
	  return EERROR;
     }
     if( stackSize < STACK_MIN )
	  stackSize = STACK_MIN;

     /* Allocate a new process structure */
     pProc = (Process *)malloc(sizeof(Process));
     if( pProc == NULL )
     {
	  ERROR("Failed to allocate process struct");
	  return EERROR;
     }

     /* Allocate a stack for the process */
     pProc->stack = (U32 *)malloc(stackSize);
     if( pProc->stack == NULL )
     {
	  ERROR("Failed to allocate process stack");
	  return EERROR;
     }

     /* Stack grows from the bottom up */
     pProc->stack += stackSize/sizeof(U32);

     /* Copy process name */
     strncpy( pProc->pcb.procName, procName, MAX_PROC_NAME_LEN );
     /* There is no guarantee that strncpy adds string terminator if len = maxLen */
     pProc->pcb.procName[MAX_PROC_NAME_LEN] = '\0';

     /* Set default values for register */
     memset( pProc->pcb.regs, 0, sizeof(U32) * PROC_NUM_REGS );
     /* Set stack pointer in r1, leave room for one U32 */
     pProc->pcb.regs[PROC_REG_STACK_PTR] = (U32)(pProc->stack - sizeof(U32));
     
     /* For now we do not handle processes exiting, so set LR to CI */
     pProc->pcb.regs[PROC_REG_LR] = 0x0;

     /* Set PC to start of process */
     pProc->pcb.regs[PROC_REG_PC] = (U32)entryPoint;
     pProc->entryPoint            =      entryPoint;

     /* Save prio */
     pProc->prio  = prio;

     /* Set pid */
     pProc->pid   = ++currPid;

     /* Set initial process state */
     pProc->state = PROC_STOPPED;

     /* Set link pointers */
     pProc->pNext      = NULL;
     pProc->pWaitNext  = NULL;
     pProc->pReadyNext = NULL;

     /* Add process to global process list */
     if( procList == NULL )
	  procList = pProc;
     else
     {
	  Process *pLink = procList;

	  while( pLink->pNext != NULL )
	       pLink = pLink->pNext;

	  pLink->pNext = pProc;
     }

     INFO(STR("Created process %s", pProc->pcb.procName));

     return pProc->pid;
}

void start_process(U32 pid)
{
     Process *pProc = find_proc(pid);

     /* No process found */
     if( pProc == NULL )
	  return;

     /* Nothing to do */
     if( pProc->state != PROC_STOPPED )
	  return;

     /* Start process */
     pProc->state = PROC_READY;

     /* Link process into ready queue for the priority level */
     if( procReadyList[pProc->prio] == NULL )
	  procReadyList[pProc->prio] = pProc;
     else
     {
	  Process *pLink = procReadyList[pProc->prio];

	  while( pLink->pReadyNext != NULL )
	       pLink = pLink->pReadyNext;

	  pLink->pReadyNext = pProc;
     }

     INFO("Started process");
}

void stop_process(U32 pid)
{
     Process *pProc = find_proc(pid);

     /* No process found */
     if( pProc == NULL )
	  return;

     /* Nothing to do */
     if( pProc->state == PROC_STOPPED )
	  return;
     if( pProc->state == PROC_RUNNING )
     {
	  /* No currently running process after this */
	  procRunning = NULL;

	  /* yeild() */
     }
     if( pProc->state == PROC_WAITING )
     {
	  Process *pLink = procWaitList;

	  pProc->state = PROC_STOPPED;

	  if( pProc == procWaitList )
	  {
	       procWaitList = pProc->pNext;
	       return;
	  }

	  while( pLink->pWaitNext != NULL )
	  {
	       if( pLink->pWaitNext == pProc )
	       {
		    pLink->pWaitNext = pLink->pWaitNext->pWaitNext;
		    return;
	       }
	       pLink = pLink->pWaitNext;
	  }
     }
     if( pProc->state == PROC_READY )
     {
	  Process *pLink = procReadyList[pProc->prio];

	  pProc->state = PROC_STOPPED;

	  if( pProc == procReadyList[pProc->prio] )
	  {
	       procReadyList[pProc->prio] = pProc->pNext;
	       return;
	  }

	  while( pLink->pReadyNext != NULL )
	  {
	       if( pLink->pReadyNext == pProc )
	       {
		    pLink->pReadyNext = pLink->pReadyNext->pReadyNext;
		    return;
	       }
	       pLink = pLink->pReadyNext;
	  }
     }
}

U32 current_process()
{
     if( procRunning != NULL )
	  return procRunning->pid;

     return 0;
}

void sleep(U32 milliSeconds)
{
     /* 
	We allow a sleep of 0 ms, which will put us
	to ready at next scheduler invocation.
	Negative values will be treated the same.
     */

     Process *pLink = procWaitList;

     procRunning->msSleep = (S32)milliSeconds;

     if( procWaitList == NULL )
	  procWaitList = procRunning;
     else
     {
	  while( pLink->pWaitNext != NULL )
	       pLink = pLink->pWaitNext;

	  pLink->pWaitNext = procRunning;
     }

     procRunning = NULL;
     /* yield() */
}

Process *find_proc(U32 pid)
{
     Process *pProc = procList;
     
     while( pProc != NULL )
     {
	  if( pProc->pid == pid )
	       return pProc;

	  pProc = pProc->pNext;
     }

     return NULL;
}

/* This gives us a process that is always available for execution */
OS_PROC(krnIdle)
{
     INFO("Kernel idle process started");

     while(1);
}
