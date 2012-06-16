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
#include "sched/scheduler.h"
#include "arch/ppc440.h"

#include <string.h>
#include <stdlib.h>
#include <sys/errno.h>

__Process *__procList        = NULL;
__Process *__procWaitList    = NULL;
__Process *__procReadyList[NUM_PRIO];
__Process *__procRunning     = NULL;

U32      currPid             = 0;

void __proc_init()
{
     memset(__procReadyList, 0, sizeof(__Process *) * NUM_PRIO);

     INFO(STR("Size: %u", sizeof(__Process)));

     INFO("Process handling initilized");
}

U32 create_process(char *procName, OSPROC entryPoint, U8 prio, U16 stackSize)
{
     __Process *pProc;

     /* Check valid variables. Stack size must be a multiple of 4 */
     if( procName == NULL || entryPoint == NULL || (prio > PRIO_MIN && entryPoint != krnIdle) || (stackSize % 4) != 0 )
     {
	  errno = EINVAL;
	  return EERROR;
     }
     if( stackSize < STACK_MIN )
	  stackSize = STACK_MIN;

     /* Allocate a new process structure */
     pProc = (__Process *)malloc(sizeof(__Process));
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
     strncpy( pProc->pcb.procName, procName, __MAX_PROC_NAME_LEN );
     /* There is no guarantee that strncpy adds string terminator if len = maxLen */
     pProc->pcb.procName[__MAX_PROC_NAME_LEN] = '\0';

     /* Set default values for register */
     memset( pProc->pcb.regs, 0, sizeof(U32) * __PROC_NUM_REGS );
     /* Set stack pointer in r1, leave room for one U32 */
     pProc->pcb.regs[__PROC_REG_STACK_PTR] = (U32)(pProc->stack - sizeof(U32)); /* FIXME -> this is actually -16 */
     
     /* For now we do not handle processes exiting, so set LR to CI */
     pProc->pcb.regs[__PROC_REG_LR] = 0x0;

     /* Set PC to start of process */
     pProc->pcb.regs[__PROC_REG_PC] = (U32)entryPoint;
     pProc->entryPoint              =      entryPoint;

     /* Save prio */
     pProc->prio  = prio;

     /* Set pid */
     pProc->pid   = ++currPid;

     /* Set initial process state */
     pProc->state = __PROC_STOPPED;

     /* Set link pointers */
     pProc->pNext      = NULL;
     pProc->pWaitNext  = NULL;
     pProc->pReadyNext = NULL;

     /* Set up default file descriptors (STDIN, STDOUT, STDERR) */
     __vfs_init_proc(pProc->pid);
     
     /* Open descriptors, ignore return values (allow failure) */
     open("/dev/tty", O_RDONLY); /* STDIN */
     open("/dev/tty", O_WRONLY); /* STDOUT */
     open("/dev/tty", O_WRONLY); /* STDERR */

     /* Add process to global process list */
     if( __procList == NULL )
	  __procList = pProc;
     else
     {
	  __Process *pLink = __procList;

	  while( pLink->pNext != NULL )
	       pLink = pLink->pNext;

	  pLink->pNext = pProc;
     }

     INFO(STR("Created process %s, pid: %u", pProc->pcb.procName, pProc->pid));

     return pProc->pid;
}

void start_process(U32 pid)
{
     __Process *pProc = __proc_find(pid);

     /* No process found */
     if( pProc == NULL )
	  return;

     /* Nothing to do */
     if( pProc->state != __PROC_STOPPED )
	  return;

     /* Start process */
     pProc->state = __PROC_READY;

     /* Link process into ready queue for the priority level */
     if( __procReadyList[pProc->prio] == NULL )
	  __procReadyList[pProc->prio] = pProc;
     else
     {
	  __Process *pLink = __procReadyList[pProc->prio];

	  while( pLink->pReadyNext != NULL )
	       pLink = pLink->pReadyNext;

	  pLink->pReadyNext = pProc;
     }

     INFO(STR("Started process %u", pProc->pid));
}

void stop_process(U32 pid)
{
     __Process *pProc = __proc_find(pid);
     U32 msr;

     /* No process found */
     if( pProc == NULL )
	  return;

     /* Nothing to do */
     if( pProc->state == __PROC_STOPPED )
	  return;
     if( pProc->state == __PROC_RUNNING )
     {
	  /* No currently running process after this */
	  __procRunning  = NULL;
	  pProc->state = __PROC_STOPPED;
	  YIELD(msr);
     }
     if( pProc->state == __PROC_WAITING )
     {
	  __Process *pLink = __procWaitList;

	  pProc->state = __PROC_STOPPED;

	  if( pProc == __procWaitList )
	  {
	       __procWaitList = pProc->pNext;
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
     if( pProc->state == __PROC_READY )
     {
	  __Process *pLink = __procReadyList[pProc->prio];

	  pProc->state = __PROC_STOPPED;

	  if( pProc == __procReadyList[pProc->prio] )
	  {
	       __procReadyList[pProc->prio] = pProc->pNext;
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
     if( __procRunning != NULL )
	  return __procRunning->pid;

     return 0;
}

void sleep(U32 milliSeconds)
{
     /* 
	We allow a sleep of 0 ms, which will put us
	to ready at next scheduler invocation.
	Negative values will be treated the same.
     */

     U32 msr;
     __Process *pLink;

     MFMSR(msr);
     WRTEEI(0);
     ISYNC;

     pLink = __procWaitList;

     __procRunning->msSleep = (S32)milliSeconds;
     __procRunning->state   = __PROC_WAITING;

     if( __procWaitList == NULL )
	  __procWaitList = __procRunning;
     else
     {
	  while( pLink->pWaitNext != NULL )
	       pLink = pLink->pWaitNext;

	  pLink->pWaitNext = __procRunning;
     }

     /* procRunning = NULL; */

     MTMSR(msr);
     ISYNC;

     YIELD(msr);
}

__Process *__proc_find(U32 pid)
{
     __Process *pProc = __procList;
     
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
