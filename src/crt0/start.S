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
#;

#; OS entry point
#;
#; .text starts at 0x01000000
#;

	.section .start,"ax"
	.global _start
_start:
	
#;	Clear MSR in case this is not a power on reset
	li	r0,0
	mtmsr	r0

#;	GCC uses r1 as a stack pointer
	lis	r1,stack_top@ha
	addi	r1,r1,stack_top@l

#;	Sync processor state and context	
	isync
	sync

#;	Branch to C-code
	b startOS