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

#include "mm/mm.h"
#include "uart/uart.h"
#include "irq/irq.h"
#include "timer/timer.h"
#include "proc/process.h"
#include "proc/test_proc.h"
#include "vfs/vfs.h"

#include "krntypes.h"

#include <stdio.h>

void startOS()
{
     U32 idlePid;

     /* Start the memory manager */
     __mm_init();

     /* Set up initial interrupt handlers */
     irq_init();

     /* Initilize the UART */
     uart_init();

     /* Set up process handling */
     __proc_init();

     /* Start system idle process */
     idlePid = create_process("krnIdle", krnIdle, __PRIO_IDLE, 1024);
     start_process(idlePid);

     /* Create some test processes */
     start_process(create_process("TestProc1", test1, 20, 1024));
     start_process(create_process("TestProc2", test2, 20, 1024));
     start_process(create_process("TestProc3", test3, 10, 1024));

     /* Set up timer, this should be the last thing done */
     fit_init();
     fit_enable();

     /* Temp call */
     open("/", O_RDONLY);

     /*
       Spin here until the first FIT triggers.
       We will never come back here again
     */
     while(1) ;
}
