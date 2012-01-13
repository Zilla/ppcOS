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

#include "mm.h"
#include "malloc.h"

/* Global declaration */

MemoryRegion *allocRegions;


/************************************************************************
 * This function initilizes the Memory Manager.
 * It will set up the internal data structures needed, and create memory
 * regions for the kernel and the MM data area.
 ************************************************************************/

void mm_init()
{
     MemoryRegion *pReg;

     /* Set up a TLB entry to cover .data + heap and stack for the kernel */
     /* Hard coded for now... */
     allocRegions = malloc(sizeof(MemoryRegion));

     pReg = allocRegions;

     pReg->vAddr    = 0xF00000;
     pReg->pAddr    = 0xF00000;
     pReg->pExtAddr = 0;
     pReg->owner    = MM_GLOBAL_REGION;
     pReg->size     = TLB_SIZE_1MB;
     pReg->perms    = TLB_PERM_SR|TLB_PERM_SW|TLB_PERM_UR;
     pReg->ts       = TLB_TS_SYSTEM;
     pReg->attr     = TLB_ATTR_NONE;
     pReg->tid      = MM_GLOBAL_REGION;

     mm_create_tlb_entry(pReg);
     mm_write_tlb_entry(0, pReg);

     /* Set up a TLB entry to cover .text for the kernel */
     /* Hard coded for now... */
     pReg->pNext = malloc(sizeof(MemoryRegion));
     pReg = pReg->pNext;

     pReg->vAddr    = 0x1000000;
     pReg->pAddr    = 0x1000000;
     pReg->pExtAddr = 0;
     pReg->owner    = MM_GLOBAL_REGION;
     pReg->size     = TLB_SIZE_1MB;
     pReg->perms    = TLB_PERM_SR|TLB_PERM_SX;
     pReg->ts       = TLB_TS_SYSTEM;
     pReg->attr     = TLB_ATTR_NONE;
     pReg->tid      = MM_GLOBAL_REGION;

     mm_create_tlb_entry(pReg);
     mm_write_tlb_entry(1, pReg);

     /*
       Clear old UTLB entries.
       The reason we do this is that having overlapping TLB entries
       can cause a machine check exception if the memory area is accessed.
     */
     mm_clear_utlb(2);

     /* Force update of STLB */
     mm_invalidate_stlb();
}

/************************************************************************
 * This function removes all entries currently in the Unified TLB
 ************************************************************************/

void mm_clear_utlb(U8 startIdx)
{
     U8 tlbIdx = startIdx;
     U32 tlbWord0;

     /* Loop through all 64 UTLB entries */
     while(tlbIdx < 64)
     {
	  /* Read the TLB entry */
	  asm volatile (
	       "tlbre %0,%1,0;"
	       : "=r"(tlbWord0)
	       : "r"(tlbIdx)
	       );

	  /* If it is marked as valid... */
	  if(tlbWord0 & TLB_VALID)
	  {
	       /* ...mark it as invalid */
	       tlbWord0 &= TLB_VALID_MASK;

	       /* Write the modified TLB entry */
	       asm volatile (
		    "tlbwe %0,%1,0;"
		    : /* No output */
		    : "r"(tlbWord0), "r"(tlbIdx)
		    );
	  }
	  tlbIdx++;
     }
}

/************************************************************************
 * This function invalidates the Shadow TLB by
 * forcing a context synchronization.
 ************************************************************************/

void mm_invalidate_stlb()
{
     asm volatile (
	  "isync;"
	  );
}

/************************************************************************
 * This function writes a TLB entry to the UTLB in the MMU.
 ************************************************************************/

int mm_write_tlb_entry(U8 tlbIdx, MemoryRegion *pReg)
{
     /* Check if the TLB words have been created */
     if(!(pReg->tlbWord0 & TLB_VALID))
	  return -1;

     /* Write the TLB entry */
     asm volatile (
	  "tlbwe %0,%3,0;"
	  "tlbwe %1,%3,1;"
	  "tlbwe %2,%3,2;"
	  : /* No output */
	  : "r"(pReg->tlbWord0), "r"(pReg->tlbWord1), "r"(pReg->tlbWord2), "r"(tlbIdx)
	  );

     return 0;
}

/************************************************************************
 * This function creates the TLB words for use by the MMU.
 ************************************************************************/

int mm_create_tlb_entry(MemoryRegion *pReg)
{
     /* Field length for (E|R)PN fields */
     U8 pnLen = 22 - pReg->size * 2;
     U8 i = 0;
     U32 pnMask = 0;

     /* Combination of cache inhibit and write through is not allowed */
     if((pReg->attr & TLB_ATTR_CACHE_INHIBIT) && (pReg->attr & TLB_ATTR_WRITE_THROUGH))
	  return -1;

     /* Create a mask for Page Numbers */
     while(i++ < pnLen)
     {
	  pnMask |= 1;
	  pnMask <<= 1;
     }
     pnMask |= 1;
     pnMask <<= (32-pnLen);

     /* Set EPN */
     pReg->tlbWord0 = pReg->vAddr;

     /* Mask off unused bits */
     pReg->tlbWord0 &= pnMask;

     /* Set valid bit */
     pReg->tlbWord0 |= TLB_VALID;

     /* Set TS */
     pReg->tlbWord0 |= (pReg->ts << 9);

     /* Set size */
     pReg->tlbWord0 |= (pReg->size << 4);

     /* Set RPN */
     pReg->tlbWord1  = pReg->pAddr;
      pReg->tlbWord1 &= pnMask; 

     /* Set ERPN */
     pReg->tlbWord1 |= pReg->pExtAddr;

     /* Set page attributes */
     pReg->tlbWord2 = (pReg->attr << 6);

     /* Set page permission */
     pReg->tlbWord2 |= pReg->perms;

     return 0;
}
