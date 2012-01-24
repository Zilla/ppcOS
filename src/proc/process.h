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

#ifndef _ppcos_process_h_
#define _ppcos_process_h_

#include "krntypes.h"

#define MAX_PROC_NAME_LEN    64

#define PRIO_MAX             0
#define PRIO_MIN             31
#define NUM_PRIO             33

#define PRIO_IDLE            32

#define PROC_NUM_REGS        40

#define STACK_MIN            128

/* Registers */
#define PROC_REG_GPR_START   0
#define PROC_REG_STACK_PTR   1
#define PROC_REG_GPR_END     32
#define PROC_REG_LR          32
#define PROC_REG_CR          33
#define PROC_REG_XER         34
#define PROC_REG_CTR         35
#define PROC_REG_PC          36

typedef enum {PROC_READY, PROC_WAITING, PROC_RUNNING, PROC_STOPPED} ProcessState;

typedef void(*OSPROC)(void);
#define OS_PROC(x) void x(void)

/* Process Control Block */
typedef struct _PCB {
     /* Administrative data */
     char  procName[MAX_PROC_NAME_LEN + 1];
     
     /* Execution data */
     U32   regs[PROC_NUM_REGS];
} PCB;

/* Process */
typedef struct _Process {
     PCB              pcb;
     U32             *stack;
     ProcessState     state;
     S32              msSleep;
     OSPROC           entryPoint;

     U32              pid;
     U8               prio;

     struct _Process *pNext;
     struct _Process *pWaitNext;
     struct _Process *pReadyNext;
} Process;

extern Process *procList;
extern Process *procWaitList;
/*extern Process *procStopList;*/
extern Process *procReadyList[];
extern Process *procRunning;

void proc_init();
U32 create_process(char *procName, OSPROC entryPoint, U8 prio, U16 stackSize);
void start_process(U32 pid);
void stop_process(U32 pid);
U32 current_process();
void sleep(U32 milliSeconds);
Process *find_proc(U32 pid);
OS_PROC(krnIdle);

#endif  /* _ppcos_process_h_ */
