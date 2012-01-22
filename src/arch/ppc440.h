/* Copyright (c) 2012, Peter Jönsson
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

#ifndef _ppcos_ppc440_h_
#define _ppcos_ppc440__h_

#define MTSPR(value, spr)         asm volatile ("mtspr %1, %0;"   : /* No output */ : "r"(value), "i"(spr));
#define MFSPR(result, spr)        asm volatile ("mfspr %0, %1"    : "=r" (result)   : "i" (spr));
#define TLBRE(result, idx, word)  asm volatile ("tlbre %0,%1,%2;" : "=r"(result)    : "r"(idx), "i"(word));
#define TLBWE(value, idx, word)   asm volatile ("tlbwe %0,%1,%2;" : /* No output */ : "r"(value), "r"(idx), "i"(word));
#define ISYNC                     asm volatile ("isync;");
#define MTMSR(value)              asm volatile ("mtmsr %0;"       : /* No output */ : "r"(value));
#define MFMSR(result)             asm volatile ("mfmsr %0"        : "=r" (result));


/* Mask for Machine Status Register (MSR) */
#define MSR_WAIT_STATE_ENABLE     0x40000
#define MSR_CRTICAL_IRQ_ENABLE    0x20000
#define MSR_EXTERNAL_IRQ_ENABLE   0x8000
#define MSR_PROBLEM_STATE         0x4000
#define MSR_FLOAT_POINT_AVAIL     0x2000
#define MSR_MACHINE_CHECK_ENABLE  0x1000
#define MSR_FLOAT_POINT_EXC_MODE0 0x800
#define MSR_DEBUG_WAIT_ENABLE     0x400
#define MSR_DEBUG_INT_ENABLE      0x200
#define MSR_FLOAT_POINT_EXC_MODE1 0x100
#define MSR_INST_ADDR_SPACE       0x20
#define MSR_DATA_ADDR_SPACE       0x10

#define IRQ_IVOR_BASE  0x0
#define IRQ_IVOR_SIZE  0x100
#define IRQ_IVOR_COUNT 0x10
#define IRQ_STACK_BASE 0x1000
#define IRQ_STACK_SIZE 0x400

/* IVOR definitions */ 
#define IRQ_IVOR_CRIT_INPUT       0
#define IRQ_IVOR_MACH_CHECK       1
#define IRQ_IVOR_DATA_STORE       2
#define IRQ_IVOR_INST_STORE       3
#define IRQ_IVOR_EXTE_INPUT       4
#define IRQ_IVOR_ALIGNMENT        5
#define IRQ_IVOR_PROGRAM          6
#define IRQ_IVOR_FP_UNAVAIL       7
#define IRQ_IVOR_SYST_CALL        8
#define IRQ_IVOR_AUX_PROC_UNAVAIL 9
#define IRQ_IVOR_DECREMENTER      10
#define IRQ_IVOR_FIXED_TIME       11
#define IRQ_IVOR_WDOG_TIME        12
#define IRQ_IVOR_DATA_TLB_ERR     13    /* Data TLB error (miss) */
#define IRQ_IVOR_INST_TLB_ERR     14
#define IRQ_IVOR_DEBUG            15

/* Special Purpose Registers */
#define CCR0        0x3b3 /* Core Configuration Register 0 */
#define CCR1        0x378 /* Core Configuration Register 1 */
#define CSRR0       0x03a /* Critical Save/Restor Register 0 */
#define CSRR1       0x03b /* Critical Save/Restor Register 1 */
#define CTR         0x009 /* Count Register */
#define DAC1        0x13c /* Data Address Compare 1 */ 
#define DAC2        0x13d /* Data Address Compare 2 */
#define DBCR0       0x134 /* Debug Control Register 0 */
#define DBCR1       0x135 /* Debug Control Register 1 */
#define DBCR2       0x136 /* Debug Control Register 2 */
#define DBDR        0x3f3 /* Debug Data Register */
#define DBSR        0x130 /* Debug Status Register */
#define DCDBTRH     0x39d /* Data Cache Debug Tag Register High */
#define DCDBTRL     0x39c /* Data Cache Debug Tag Register Low */
#define DEAR        0x03d /* Data Exception Address Register */
#define DEC         0x016 /* Decrement */
#define DECAR       0x036 /* Decrement Auto-Reload */
#define DNV0        0x390 /* Data Cache Normal Victim 0 */
#define DNV1        0x391 /* Data Cache Normal Victim 1 */
#define DNV2        0x392 /* Data Cache Normal Victim 2 */
#define DNV3        0x393 /* Data Cache Normal Victim 3 */
#define DTV0        0x394 /* Data Cache Transient Victim 0 */
#define DTV1        0x395 /* Data Cache Transient Victim 1 */
#define DTV2        0x396 /* Data Cache Transient Victim 2 */
#define DTV3        0x397 /* Data Cache Transient Victim 3 */
#define DVC1        0x13e /* Data Value Compare 1 */
#define DVC2        0x13f /* Data Value Compare 2 */
#define DVLIM       0x398 /* Data Cache Victim Limit */
#define ESR         0x03e /* Exception Syndrome Register */
#define IAC1        0x138 /* Instruction Address Compare 1 */
#define IAC2        0x139 /* Instruction Address Compare 2 */
#define IAC3        0x13a /* Instruction Address Compare 3 */
#define IAC4        0x13b /* Instruction Address Compare 4 */
#define ICDBDR      0x3d3 /* Instruction Cache Debug Data Register */
#define ICDBRL      0x39f /* Instruction Cache Debug Register High */
#define ICDBRH      0x39e /* Instruction Cache Debug Register Low */
#define INVO        0x370 /* Instruction Cache Normal Victim 0 */
#define INV1        0x371 /* Instruction Cache Normal Victim 1 */
#define INV2        0x372 /* Instruction Cache Normal Victim 2 */
#define INV3        0x373 /* Instruction Cache Normal Victim 3 */
#define ITV0        0x374 /* Instruction Cache Transient Victim 0 */
#define ITV1        0x375 /* Instruction Cache Transient Victim 1 */
#define ITV2        0x376 /* Instruction Cache Transient Victim 2 */
#define ITV3        0x377 /* Instruction Cache Transient Victim 3 */
#define IVLIM       0x399 /* Instruction Cache Victim Limit */
#define IRQ_IVOR0   0x190 /* Interrupt Vector Offset Register 0 */
#define IRQ_IVOR1   0x191 /* Interrupt Vector Offset Register 1 */
#define IRQ_IVOR2   0x192 /* Interrupt Vector Offset Register 2 */
#define IRQ_IVOR3   0x193 /* Interrupt Vector Offset Register 3 */
#define IRQ_IVOR4   0x194 /* Interrupt Vector Offset Register 4 */
#define IRQ_IVOR5   0x195 /* Interrupt Vector Offset Register 5 */
#define IRQ_IVOR6   0x196 /* Interrupt Vector Offset Register 6 */
#define IRQ_IVOR7   0x197 /* Interrupt Vector Offset Register 7 */
#define IRQ_IVOR8   0x198 /* Interrupt Vector Offset Register 8 */
#define IRQ_IVOR9   0x199 /* Interrupt Vector Offset Register 9 */
#define IRQ_IVOR10  0x19A /* Interrupt Vector Offset Register 10 */
#define IRQ_IVOR11  0x19B /* Interrupt Vector Offset Register 11 */
#define IRQ_IVOR12  0x19C /* Interrupt Vector Offset Register 12 */
#define IRQ_IVOR13  0x19D /* Interrupt Vector Offset Register 13 */
#define IRQ_IVOR14  0x19E /* Interrupt Vector Offset Register 14 */
#define IRQ_IVOR15  0x19F /* Interrupt Vector Offset Register 15 */
#define IVPR        0x03f /* Interrupt Vector Prefix Register */
#define LR          0x008 /* Link Register */
#define MCSR        0x23c /* Machine Check Status Register */
#define MCSRR0      0x23a /* Machine Check Status Restore Register 0 */
#define MCSRR1      0x23b /* Machine Check Status Restore Register 1 */
#define MMUCR       0x3b2 /* Memory Management Unit Control Register */
#define PID         0x030 /* Process ID */
#define PIR         0x11e /* Processor ID Register */
#define PVR         0x11f /* Processor Version Register */
#define RSTCFG      0x39b /* Reset Configuration */
#define SPRG0       0x110 /* Special Purpose Register General 0 */
#define SPRG1       0x111 /* Special Purpose Register General 1 */
#define SPRG2       0x112 /* Special Purpose Register General 2 */
#define SPRG3       0x113 /* Special Purpose Register General 3 */
#define SPRG4_READ  0x104 /* Special Purpose Register General 4 Read */
#define SPRG4_WRITE 0x114 /* Special Purpose Register General 4 Write */
#define SPRG5_READ  0x105 /* Special Purpose Register General 5 Read */
#define SPRG5_WRITE 0x115 /* Special Purpose Register General 5 Write */
#define SPRG6_READ  0x106 /* Special Purpose Register General 6 Read */
#define SPRG6_WRITE 0x116 /* Special Purpose Register General 6 Write */
#define SPRG7_READ  0x107 /* Special Purpose Register General 7 Read */
#define SPRG7_WRITE 0x117 /* Special Purpose Register General 7 Write */
#define SRR0        0x01a /* Save/Restore Register 0 */
#define SRR1        0x01b /* Save/Restore Register 1 */
#define TBL_READ    0x10c /* Time Base Lower User for reading */
#define TBL_WRITE   0x11c /* Time Base Lower Supervisor for writing */
#define TBU_READ    0x10d /* Time Base Upper User for reading */ 
#define TBU_WRITE   0x11d /* Time Base Upper Supervisor for writing */
#define TCR         0x154 /* Timer Control Register */
#define TSR         0x150 /* Time Status Register */
#define USPRG0      0x100 /* USer Special Purpose Register General 0 */
#define XER         0x001 /* Integer Exception Register */

#define TSR_ENW 0x80000000
#define TSR_WIS 0x40000000
#define TSR_DIS 0x8000000
#define TSR_FIS 0x4000000

// TODO: fix me. See p. 180
#define TSR_WRS_NO_RESET   0x0
#define TSR_WRS_CORE_RESET 0x0
#define TSR_WRS_CHIP_RESET 0x0 
#define TSR_WRS_SYS_RESET  0xx

#define WDOG_TIME_PERIOD_1         0x00000000
#define WDOG_TIME_PERIOD_2         0x40000000
#define WDOG_TIME_PERIOD_3         0x80000000
#define WDOG_TIME_PERIOD_4         0xC0000000

#define WDOG_RESET_CTRL_CHIP_RESET 0x10000000
#define WDOG_RESET_CTRL_CORE_RESET 0x20000000
#define WDOG_RESET_CTRL_SYS_RESET  0x30000000

#define WDOG_INT_ENABLE            0x8000000
#define DEC_INT_ENABLE             0x4000000

#define FIT_TIME_PERIOD_1          0x0000000
#define FIT_TIME_PERIOD_2          0x3000000
#define FIT_TIME_PERIOD_3          0x2000000
#define FIT_TIME_PERIOD_4          0x1000000

#define FIT_INT_ENABLE             0x800000
#define AUTO_RELOAD_ENABLE         0x400000

/* TLB */
#define NUM_TLB_ENTRIES  64

#endif
