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

#ifndef _ppcos_scheduler_h_
#define _ppcos_scheduler_h_

#include "krntypes.h"

/*
  YIELD macro.
  Store r1 (stack pointer) and LR in reg backup area.
  Copy MSR to SRR1
  Disable EE
  Copy PC to SRR0
  Call yeild() to invoke scheduler
  Restore r1 and LR
  Call rfi
*/

/* Needs to be improved, small window where FIT can interrupt us = VERY BAD */

/* Extremley ugly horrible hack to get PC after the macro */
/* TODO: Rewrite... */

#define YIELD() \
     asm volatile("stw 1,0x1004(0); mflr 1; stw 1,0x1000(0);"); \
     asm volatile("mfmsr 1; mtsrr1 1; wrteei 0;"); \
     goto _savepc; \
_yield: \
     asm volatile("lwz 1,0x1004(0)"); \
     yield(); \
     asm volatile("lwz 1,0x1000(0); mtlr 1; lwz 1,0x1004(0); rfi"); \
     goto _endYield; \
_savepc: \
     asm volatile("bl 0x4; mflr 1; addi 1,1,36; mtsrr0 1");	\
     goto _yield; \
_endYield:

void sched_invoke(bool isInterrupt);
void yield();

#endif  /* _ppcos_scheduler_h_ */
