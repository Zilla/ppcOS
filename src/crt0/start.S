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

/* Reset vector instruction */
	.section .reset,"ax"
	.global _reset
_reset:
	b	_init_tlb

/* Initial setup of TLBs */
	.section .init_tlb,"ax"
	.global _init_tlb
	
_init_tlb:
	/*
	Configure two 1MB TLB entries for the kernel.
	
	TLB 0 will cover the data section,
		and maps 0xFFE00000 to 0x1FFE00000
	TLB 1 will cover the text section,
		and maps 0xFFF00000 to 0x1FFF00000
	
	The memory manager will reconfigure the TLBs when started
	*/

	/*
	Each TLB entry consists of 96 + 8 bits of information.
	
	Since we cannot write this in one operation, three TLB words will
	be constructed, and written to the TLB one at a time. The extra
	8 bytes are written together with word 0, and comes from MMUCR[STID].
	*/

	/*
	Build word 0 in r0.
	
	Word 0 consists of the following components:
        EPN:V:TS:SIZE:TPAR:TID

	EPN is the Effective Page Number, and is the base of the virtual
	address that the page relates to. The EPN field is 22 bits wide,
	but only 32 - log2(page_size) bits are used. We will set bit 0:11
	to 0xFFE for our data page.

	V is the valid bit. We will set this to 1.

	TS is the Translation Address Space. By convention, TS = 0 is used
	for system pages, and TS = 1 for user space pages. We will set TS to 0.

	SIZE is the page size. This selects one of the pre-defined Book-E
	page sizes. We will set the size to 0b0101 (5), which is a 1MB page.

	TPAR is the Tag Parity. This is ignored at write.

	TID is the Translation ID, and identifies the process owning the page.
	Global pages have TID = 0, and this is what we will use.
	This value is not set in r0, but rather in MMUCR[STID].
	*/

	/* EPN */
	li	10,0xFFE
	slwi	0,10,20

	/* V */
	li	10,1
	slwi	10,10,9
	or	0,0,10

	/* SIZE */
	li	10,5
	slwi	10,10,4
	or	0,0,10

	/* TID */
	mfspr	10,0x3B2
	rlwinm  10,10,0,0,23	/* Mask out bit 24:31 */
	mtspr	0x3B2,10

	/*
	Build word 1 in r1.
	
	Word 1 consists of the following components:
        RPN:PAR1:EPN

	RPN is the Real Page Number, and is the base of the physical
	address that the page relates to. The rules for RPN is the same
	as for EPB above. We will set bit 0:11 to 0xFFE for our data page.

	PAR1 are the parity bits for page 1, and are ignored at write.

	EPN is the Extended Real Page Number, and is used together with RPN
	and the Effective Address (EA) to create a 36-bit physical address.
	Since our data is stored at the 33-bit address 0x1_FFE0_0000, we
	set this to 1.
	*/

	/* RPN */
	li	10,0xFFE
	slwi	1,10,20

	li	10,1
	or	1,1,10

	/*
	Build word 2 in r2.

	Word 2 consists of the following components:
	PAR2:U[0-4]:W:I:M:G:E:U[XWR]:S[XWR]

	PAR2 are the parity bits for page 2, and are ignored at write.

	U0 - U4 are the User-Definable Storage Attributes
	These are free for use by the operating system, and we
	will not set them here.

	W is the Write-Through bit (caching). We set this to 0

	I is the Cache Inhibit bit. We set this to 1 (no cache)

	M is the Memory Coherence Required bit, and is only useful
	in multi core systems. We set this to 0.

	G is the Guarded bit, and prevents speculative access, i.e.
	out-of-order instruction and data fetch. We set this to 1.

	E is the Endian bit. We set it to 0 (Big Endian).

	UX,UW,UR are the User Execute, Write and Read permission bits.
	We set all of these to 0.

	SX,SW,SR are the Supervisor Execute, Write and Read permission bits.
	We set all of these to 1.
	*/

	/* I */
	li	10,1
	slwi	2,10,10

	/* G */
	li	10,1
	slwi	10,10,8
	or	2,2,10

	/* SX, SW, SR */
	li	10,7 		/* 0b111 */
	or	2,2,10

	/* Write TLB Entry 0, Word 0 */
	li	4,0
	tlbwe	0,4,0

	/* Write TLB Entry 0, Word 1 */
	tlbwe	1,4,1

	/* Write TLB Entry 0, Word 2 */
	tlbwe	2,4,2

	/*isync*/

	/*
	The TLB words for entry 1 will be the same as for entry 0,
	except that EPN and RPN will be set to 0xFFF
	*/

	/* Modify EPN */
	li	10,1
	slwi	10,10,20
	or	0,0,10

	/* Modify RPN */
	or	1,1,10

	/* Write TLB Entry 1, Word 0 */
	li	4,1
	tlbwe	0,4,0

	/* Write TLB Entry 1, Word 1 */
	tlbwe	1,4,1

	/* Write TLB Entry 1, Word 2 */
	tlbwe	2,4,2

	isync
	
	b 	_start

/* OS entry point
 *
 * .text starts at 0xFFF00000
 */

	.section .start,"ax"
	.global _start
_start:
	/* GCC will use r1 as a hint where to put function data */
	li 	1,0xFFF
	slwi	1,1,20
	li	10,0x300
	or	1,1,10
	
	b startOS