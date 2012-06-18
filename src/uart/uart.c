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

#include "uart/uart.h"
#include "krntypes.h"
#include "mm/mm.h"
#include "log/log.h"
#include "arch/ppc440.h"

#include <stdio.h>
#include <sys/errno.h>
#include <sys/unistd.h>

/* Globals */

U8 uartInitDone = 0;

/* Initilize the UART */

void uart_init()
{
     U8 reg;

     /* Map the UART area */
     __mm_map_region(UART_VBASE, UART_PBASE, UART_ERPN, 1024,
		     TLB_PERM_SR|TLB_PERM_SW, TLB_ATTR_GUARDED|TLB_ATTR_CACHE_INHIBIT, 0, TLB_SIZE_1KB);

     /* Enable DLAB */
     reg = UART_READ(UART0_LCR);
     reg |= UART_LCR_DLAB_ENABLE;
     UART_WRITE(UART0_LCR, reg);

     /* Write divisor values */
     UART_WRITE(UART0_DLL, UART_DLL_9600);
     UART_WRITE(UART0_DLM, UART_DLM_9600);

     /* Disable DLAB */
     reg = UART_READ(UART0_LCR);
     reg &= UART_LCR_DLAB_MASK;
     UART_WRITE(UART0_LCR, reg);

     /* Set serial port parameters */
     reg = UART_READ(UART0_LCR);
     reg |= UART_LCR_8BIT_WORD|UART_LCR_PARITY_ODD;
     UART_WRITE(UART0_LCR, reg);

     /* Enable FIFO */
     reg = UART_READ(UART0_FCR);
     reg |= UART_FCR_ENABLE;
     UART_WRITE(UART0_FCR, reg);

     uartInitDone = 1;
     INFO("UART initilized");
}

int uart_write(const void *buf, int len)
{
     U32 written = 0, msr;
     U8 *cPtr = (U8 *)buf;
     
     if( uartInitDone == 0 )
     {
	  errno = EIO;
	  return EERROR;
     }

     MFMSR(msr);
     WRTEEI(0);
     ISYNC;

     if( len <= 0 )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     do {
	  UART_WAIT(UART0_LSR);
	  UART_WRITE(UART0_THR, *cPtr);
	  if(*cPtr =='\n')
	  {
	       UART_WAIT(UART0_LSR);
	       UART_WRITE(UART0_THR, '\r');
	  }
	  cPtr++;
     } while(++written != len);

     MTMSR(msr);
     ISYNC;

     return written;
}
