#; Copyright (c) 2011, Joakim Östlund
#; All rights reserved.		
#;
#; Redistribution and use in source and binary forms, with or without
#; modification, are permitted provided that the following conditions are met: 
#;    * Redistributions of source code must retain the above copyright
#;      notice, this list of conditions and the following disclaimer.
#;    * Redistributions in binary form must reproduce the above copyright
#;      notice, this list of conditions and the following disclaimer in the
#;      documentation and/or other materials provided with the distribution.
#;    * The name of the author may not be used to endorse or promote products
#;      derived from this software without specific prior written permission. 
#;
#; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
#; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#; Reset vector instruction 
	.section .reset,"ax"
	.global _reset
_reset:
	b	_init_tlb

#; Initial setup of TLBs 
	.section .init_tlb,"ax"
	.global _init_tlb
	
_init_tlb:
	
#;	Configure 1MB TLB entries for the loader and kernel.
#;	
#;	TLB 0 will cover the boot loader,
#;		and maps 0xFFF00000 to 0x1FFF00000
#;	TLB 1 will cover the first half of the kernel binary in flash,
#;		and maps 0xFFC00000 to 0x1FFC00000
#;	TLB 2 will cover the second half of the kernel binary in flash,
#;		and maps 0xFFD00000 to 0x1FFD00000
#;	TLB 3 will cover the data area of the kernel in RAM,
#;		and maps 0x00F00000 to 0x00F00000
#;	TLB 4 will cover the text area of the kernel in RAM,
#;		and maps 0x01000000 to 0x01000000
#;	TLB 5 will cover the initial heap for the kernel in RAM,
#;		and maps 0x01100000 to 0x01100000

#;	
#;	The memory manager will reconfigure the TLBs when started

	
#;	Map the boot loader flash area to TBL 0
#;	Since our data is stored at the 33-bit physical address 0x1_FFF0_0000,
#;	we set ERPN to 1
	li	r0,0xFFF
	li	r1,0xFFF
	li	r2,0x1
	li	r3,0
	bl	_create_tlb_entry

	
#;	The kernel image is stored in a 2MB flash area. We create two
#;	TLB entries (1 and 2) to cover this area.   
	li	r0,0xFFC
	li	r1,0xFFC
	li	r3,1
	bl	_create_tlb_entry

	li	r0,0xFFD
	li	r1,0xFFD
	li	r3,2
	bl	_create_tlb_entry

	
#;	Map up the RAM areas where the kernel image will be copied
#;	We will map the virtual address to the same physical address,
#;	thus we set ERPN to 0.
	li	r0,0x00F
	li	r1,0x00F
	li	r2,0x0
	li	r3,3
	bl	_create_tlb_entry

	li	r0,0x010
	li	r1,0x010
	li	r3,4
	bl	_create_tlb_entry
	
	li	r0,0x011
	li	r1,0x011
	li	r3,5
	bl	_create_tlb_entry

#;      Force CPU to update shadow TLB
	isync
	
	b 	_copy_kernel_image

#;************************************************************
#; This function copies the kernel image from flash to RAM.
#; For now, everything is hardcoded.
#; Also, this copy rutine is horribly ineffective.
#;************************************************************

_copy_kernel_image:

#;	Load start address into r1
	li	r1,0xFFC
	slwi	r1,r1,20

#;	Load end address into r2	
	li	r2,0xFFE
	slwi	r2,r2,20
	subi	r2,r2,4
	
#;	Load target address into r3
	li	r3,0x00F
	slwi	r3,r3,20

#;	Copy first word
	lwz	10,0(r1)
	stw	10,0(r3)

_copy_loop:
#;	Loop and copy the rest of the words
	cmpw	r1,r2		#; Check if copy is done
	beq	_start_kernel	#; Start kernel	
	lwzu	10,4(r1)
	stwu	10,4(r3)
	b	_copy_loop

#;************************************************************
#; This function branches to (executes) the kernel start point.
#; For now, everything is hardcoded.
#;************************************************************

_start_kernel:
	
#;	Invalidate Link Register to avoid accidents.
	li	r0,0
	mtlr	r0
	
#;	Branch to kernel start point
	ba	0x01000000
	
#;************************************************************
#; This function builds a TLB entry and writes it to the MMU
#; The input paramters are:
#; r0 - EPN
#; r1 - RPN
#; r2 - ERPN
#; r3 - TLB index
#;
#; The input registers will not be touched. However,
#; r5, r6, r7 and r10 will be overwritten
#;************************************************************
	
_create_tlb_entry:
	
#;	Each TLB entry consists of 96 + 8 bits of information.
#;	
#;	Since we cannot write this in one operation, three TLB words will
#;	be constructed, and written to the TLB one at a time. The extra
#;	8 bytes are written together with word 0, and comes from MMUCR[STID].
	

#;	Build word 0 in r5.
#;	
#;	Word 0 consists of the following components:
#;      EPN:V:TS:SIZE:TPAR:TID
#;
#;	EPN is the Effective Page Number, and is the base of the virtual
#;	address that the page relates to. The EPN field is 22 bits wide,
#;	but only 32 - log2(page_size) bits are used. We will set bit 0:11
#;	to the value of r0
#;
#;	V is the valid bit. We will set this to 1.
#;
#;	TS is the Translation Address Space. By convention, TS = 0 is used
#;	for system pages, and TS = 1 for user space pages. We will set TS to 0.
#;
#;	SIZE is the page size. This selects one of the pre-defined Book-E
#;	page sizes. We will set the size to 0b0101 (5), which is a 1MB page.
#;
#;	TPAR is the Tag Parity. This is ignored at write.
#;
#;	TID is the Translation ID, and identifies the process owning the page.
#;	Global pages have TID = 0, and this is what we will use.
#;	This value is not set in r5, but rather in MMUCR[STID].
	

#;	EPN 
	slwi	r5,r0,20

#;	V 
	li	r10,1
	slwi	r10,r10,9
	or	r5,r5,10

#;	SIZE 
	li	r10,5
	slwi	r10,r10,4
	or	r5,r5,10

#;	TID 
	mfspr	r10,0x3B2
	rlwinm  r10,r10,0,0,23	#; Mask out bit 24:31 
	mtspr	0x3B2,r10

	
#;	Build word 1 in r6.
#;	
#;	Word 1 consists of the following components:
#;      RPN:PAR1:EPN
#;
#;	RPN is the Real Page Number, and is the base of the physical
#;	address that the page relates to. The rules for RPN is the same
#;	as for EPB above. We will set bit 0:11 to the value of r2.
#;
#;	PAR1 are the parity bits for page 1, and are ignored at write.
#;
#;	ERPN is the Extended Real Page Number, and is used together with RPN
#;	and the Effective Address (EA) to create a 36-bit physical address.
#;	We will set this to the value of r3.


#;	RPN
	slwi	r6,r1,20

#;	ERPN
	or	r6,r6,2

	
#;	Build word 2 in r7.
#;
#;	Word 2 consists of the following components:
#;	PAR2:U[0-4]:W:I:M:G:E:U[XWR]:S[XWR]
#;
#;	PAR2 are the parity bits for page 2, and are ignored at write.
#;
#;	U0 - U4 are the User-Definable Storage Attributes
#;	These are free for use by the operating system, and we
#;	will not set them here.
#;
#;	W is the Write-Through bit (caching). We set this to 0
#;
#;	I is the Cache Inhibit bit. We set this to 1 (no cache)
#;
#;	M is the Memory Coherence Required bit, and is only useful
#;	in multi core systems. We set this to 0.
#;
#;	G is the Guarded bit, and prevents speculative access, i.e.
#;	out-of-order instruction and data fetch. We set this to 1.
#;
#;	E is the Endian bit. We set it to 0 (Big Endian).
#;
#;	UX,UW,UR are the User Execute, Write and Read permission bits.
#;	We set all of these to 0.
#;
#;	SX,SW,SR are the Supervisor Execute, Write and Read permission bits.
#;	We set all of these to 1.

#;	I
	li	r10,1
	slwi	r7,r10,10

#;	G
	li	r10,1
	slwi	r10,r10,8
	or	r7,r7,10

#;	SX, SW, SR
	li	r10,7 		#; 0b111 
	or	r7,r7,10

#;	Write TLB Entry, Word 0
	tlbwe	r5,r3,0

#;	Write TLB Entry, Word 1
	tlbwe	r6,r3,1

#;	Write TLB Entry, Word 2
	tlbwe	r7,r3,2

#;	Return to caller 
	blr

