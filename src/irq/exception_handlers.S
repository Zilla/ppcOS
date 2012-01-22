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

#; Assembly rutines for various PPC exceptions.
#; The responsibility of the handlers will be to save and restore registers,
#; then call the C-function that does the actual handling of the interrupt.
#;
#; It might be possible to do a generic version of this, but keep them separate for now.
#;
#; IMPORTANT:
#; Use ONLY relative jumps within the handlers, as they will be copied around.	

        .section .ivor,"ax"


        .global _ivor_critical_int
_ivor_critical_int:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)
	
#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_critical_int

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfci

	.global _ivor_machine_check
_ivor_machine_check:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_machine_check

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfmci

	.global _ivor_data_storage
_ivor_data_storage:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)	
	
#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_data_storage

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_instruction_storage
_ivor_instruction_storage:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)
	
#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_instruction_storage

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_external_input
_ivor_external_input:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_external_input

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_alignment
_ivor_alignment:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_alignment

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_program
_ivor_program:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_program

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_fp_unavail
_ivor_fp_unavail:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_fp_unavail

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_system_call
_ivor_system_call:
#;	Backup r1
	stw	r1,0x1004(0)
	
#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_system_call

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_ap_unavail
_ivor_ap_unavail:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_ap_unavail

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_decrementer
_ivor_decrementer:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)
	
#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_decrementer
	
#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_fixed_interval_timer
_ivor_fixed_interval_timer:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_fixed_interval_timer

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)
	
	rfi

	.global _ivor_watchdog_timer
_ivor_watchdog_timer:
#;	Backup r1
	stw	r1,0x1004(0)
	
#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_watchdog_timer

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfci
	
        .global _ivor_data_tlb_error
_ivor_data_tlb_error:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)
	
#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_data_tlb_error

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_instruction_tlb_error
_ivor_instruction_tlb_error:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)
	
#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_instruction_tlb_error

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfi

	.global _ivor_debug
_ivor_debug:
#;	Backup r1
	stw	r1,0x1004(0)

#;	Backup LR
	mflr    r1
	stw	r1,0x1000(0)

#;	Set stack pointer
        li	r1,0x12F0
	
	bla	ivor_debug

#;	Restore LR
	lwz	r1,0x1000(0)
	mtlr    r1
	
#;	Restore r1
	lwz	r1,0x1004(0)

	rfci