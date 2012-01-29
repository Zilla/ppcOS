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

#ifndef _ppcos_uart_h_
#define _ppcos_uart_h_

#define UART_VBASE 0xE0000000
#define UART_PBASE 0x40000000
#define UART_ERPN  0x1

#define UART0_OFF 0x200
#define UART1_OFF 0x300

#define UART0	(UART_VBASE + UART0_OFF)
#define UART1	(UART_VBASE + UART1_OFF)

/* Registers for UART0 */
#define UART0_RBR    UART0
#define UART0_THR    UART0
#define UART0_DLL    UART0
#define UART0_IER    (UART0 + 1)
#define UART0_DLM    (UART0 + 1)
#define UART0_IIR    (UART0 + 2)
#define UART0_FCR    (UART0 + 2)
#define UART0_LCR    (UART0 + 3)
#define UART0_MCR    (UART0 + 4)
#define UART0_LSR    (UART0 + 5)
#define UART0_MSR    (UART0 + 6)
#define UART0_SCR    (UART0 + 7)

/* Interrupt Enable bits */
#define UART_IER_RDA   0x1 /* Received data available */
#define UART_IER_THRE  0x2 /* Transmitter holding register empty */
#define UART_IER_RLSRC 0x4 /* Receiver line status register change */
#define UART_IER_MSRC  0x8 /* Modem status register change */

/* Line Control Register bits */
#define UART_LCR_5BIT_WORD   0x0
#define UART_LCR_6BIT_WORD   0x1
#define UART_LCR_7BIT_WORD   0x2
#define UART_LCR_8BIT_WORD   0x3

#define UART_LCR_2STOP_BIT   0x4
#define UART_LCR_1STOP_MASK  ~(UART_LCR_2STOP_BIT)

#define UART_LCR_NO_PAR_MASK ~(0x8+0x10+0x20)
#define UART_LCR_PARITY_ODD  0x8
#define UART_LCR_PARITY_EVEN 0x18
#define UART_LCR_PARITY_HIGH 0x28
#define UART_LCR_PARITY_LOW  0x38

#define UART_LCR_BREAK_ENABLE 0x40
#define UART_LCR_BREAK_MASK   ~(UART_LCR_BREAK_ENABLE)

#define UART_LCR_DLAB_ENABLE 0x80
#define UART_LCR_DLAB_MASK   ~(UART_LCR_DLAB_ENABLE)

/* Devisior latch values */
#define UART_DLL_9600 0x0C
#define UART_DLM_9600 0x00
#define UART_DLL_115K 0x01
#define UART_DLM_115K 0x00

/* Line Status Register */
#define UART_LSR_DATA       0x1 /* Data available */
#define UART_LSR_OVERRUN    0x2 /* Overrun error */
#define UART_LSR_PARITY_ERR 0x4 /* Parity error */
#define UART_LSR_FRAME_ERR  0x8 /* Framing error */
#define UART_LSR_BREAK      0x10 /* Break signal received */
#define UART_LSR_EMPTY      0x20 /* THR is empty */
#define UART_LSR_IDLE       0x40 /* THR is empty, and line is idle */
#define UART_LSR_FIFO_ERR   0x80 /* Errornous data in FIFO */

/* FIFO control register */
#define UART_FCR_ENABLE     0x1 /* Enable FIFO */
#define UART_FCR_CLEAR_RX   0x2 /* Clear receive FIFO */
#define UART_FCR_CLEAR_TX   0x4 /* Clear transmit FIFO */
#define UART_FCR_DMA_MODE   0x8 /* Select DMA mode 1 */

/* Receive FIFO interrupt trigger level, bytes */
#define UART_FCR_RX_INT_1   0x0
#define UART_FCR_RX_INT_4   0x40
#define UART_FCR_RX_INT_8   0x80
#define UART_FCR_RX_INT_14  0x60

/* Macros */
#define UART_WRITE(reg,val) (*((volatile U8 *)(reg)) = val)
#define UART_READ(reg) (*((volatile U8 *)(reg)))
#define UART_WAIT(reg) while(!(UART_READ(reg) & UART_LSR_EMPTY));

void uart_init();

#endif  /* _ppcos_uart_h_ */
