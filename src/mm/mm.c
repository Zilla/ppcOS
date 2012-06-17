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
#include "log/log.h"
#include <malloc.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>

/* Local defines */
#define __MM_SIZE_1MB 1048576

/* Global declarations */
MemoryRegion *allocRegions = NULL;
MemoryRegion *loadedPages[__MM_MAX_ACTIVE_TLBS];

char *heap_end = 0;
char *curr_heap_top = 0;

/* Use this until we have decided on a page replacement strategy */
U8 currTlbIdx = 0;

/******************* FUNCTIONS *********************/

/* Used by malloc() in newlib */

caddr_t sbrk(int incr)
{
     extern char heap_low; /* Defined by the linker */
     extern char heap_top; /* Defined by the linker */
     char *prev_heap_end;

     if( curr_heap_top == 0 )
	  curr_heap_top = &heap_top;
	 
     if( heap_end == 0 )
     {
	  heap_end = &heap_low;
     }

     prev_heap_end = heap_end;
	 
     while( heap_end + incr > curr_heap_top )
     {
	  /* Grab another 1MB of memory for heap */
	  __mm_map_region((int)curr_heap_top, (int)curr_heap_top, 0, __MM_SIZE_1MB,
			  TLB_PERM_SR|TLB_PERM_SW|TLB_PERM_SX,
			  TLB_ATTR_NONE, __MM_WRITE_TLB, TLB_SIZE_1MB);

	  curr_heap_top += 0x100000;
     }
     
     heap_end += incr;
     return (caddr_t) prev_heap_end;
}

/******************* INTERNAL FUNCTIONS *********************/

/************************************************************************
 * This function initilizes the Memory Manager.
 * It will set up the internal data structures needed, and create memory
 * regions for the kernel and the MM data area.
 ************************************************************************/

void __mm_init()
{
     /* Clear the list of loaded pages */
     memset(loadedPages, 0, sizeof(MemoryRegion *));

     /* Set up a TLB entry to cover .data + heap and stack for the kernel */
     /* Hard coded for now... */
     __mm_map_region(0xF00000, 0xF00000, 0, __MM_SIZE_1MB,
		   TLB_PERM_SR|TLB_PERM_SW|TLB_PERM_UR,
		   TLB_ATTR_NONE, __MM_LOCK_TLB|__MM_WRITE_TLB, TLB_SIZE_1MB);

     /* Set up a TLB entry to cover .text for the kernel */
     /* Hard coded for now... */
     __mm_map_region(0x1000000, 0x1000000, 0, __MM_SIZE_1MB,
		   TLB_PERM_SR|TLB_PERM_SX,
		   TLB_ATTR_NONE, __MM_LOCK_TLB|__MM_WRITE_TLB, TLB_SIZE_1MB);

     /* Set up a TLB entry to cover the initial heap for the kernel */
     /* Hard coded for now... */
     __mm_map_region(0x1100000, 0x1100000, 0, __MM_SIZE_1MB,
		   TLB_PERM_SR|TLB_PERM_SW|TLB_PERM_SX,
		   TLB_ATTR_NONE, __MM_LOCK_TLB|__MM_WRITE_TLB, TLB_SIZE_1MB);

     /*
       Clear old UTLB entries.
       The reason we do this is that having overlapping TLB entries
       can cause a machine check exception if the memory area is accessed.
     */
     __mm_invalidate_utlb(true);

     /* Force update of STLB */
     __mm_invalidate_stlb();
}

/************************************************************************
 * This function removes all entries currently in the Unified TLB
 ************************************************************************/

void __mm_invalidate_utlb(bool preserveLocked)
{
     U8 tlbIdx = 0;
     U32 tlbWord0, tlbWord2;

     /* Loop through all applicable UTLB entries */
     while(tlbIdx < __MM_MAX_ACTIVE_TLBS)
     {
	  /* Read word 2 of the TLB entry */
	  TLBRE(tlbWord2, tlbIdx, 2);
	  
	  /* Check if entry is locked, and if we care */
	  if(preserveLocked &&
	     (tlbWord2 & (__MM_TLB_LOCKED << __MM_SH_LOCK)))
	  {
	       tlbIdx++;
	       continue;
	  }

	  /* Read word 0 of the TLB entry */
	  TLBRE(tlbWord0, tlbIdx, 0);

	  /* Mark it as invalid */
	  tlbWord0 &= ~TLB_VALID;

	  /* Write the modified TLB entry */
	  TLBWE(tlbWord0, tlbIdx, 0);
	  
	  tlbIdx++;
     }
}

/************************************************************************
 * This function invalidates the Shadow TLB by
 * forcing a context synchronization.
 ************************************************************************/

void __mm_invalidate_stlb()
{
     ISYNC; 
}

/************************************************************************
 * This function writes a TLB entry to the UTLB in the MMU.
 ************************************************************************/

int __mm_write_tlb_entry(U8 tlbIdx, MemoryRegion *pReg)
{
     U32 tid, mask;

     /* Check if the TLB words have been created */
     if(!(pReg->tlbWord0 & TLB_VALID))
     {
	  errno = EINVAL;
	  return EERROR;
     }

     /* Set TID */
     MFSPR(tid, MMUCR);
     mask = ~0 << 8;
     tid &= mask;
     tid |= pReg->tid;
     MTSPR(tid, MMUCR);

     /* Write the TLB entry */     
     TLBWE(pReg->tlbWord0, tlbIdx, 0);
     TLBWE(pReg->tlbWord1, tlbIdx, 1);
     TLBWE(pReg->tlbWord2, tlbIdx, 2);

     pReg->loaded = __MM_TLB_LOADED;
     loadedPages[tlbIdx] = pReg;

     return EOK;
}

/************************************************************************
 * This function creates the TLB words for use by the MMU.
 ************************************************************************/

int __mm_create_tlb_entry(MemoryRegion *pReg)
{
     /* Field length for (E|R)PN fields */
     U8 pnLen = 22 - pReg->size * 2;
     U8 i = 0;
     U32 pnMask = 0;

     /* Combination of cache inhibit and write through is not allowed */
     if((pReg->attr & TLB_ATTR_CACHE_INHIBIT) && (pReg->attr & TLB_ATTR_WRITE_THROUGH))
     {
	  errno = EINVAL;
	  return EERROR;
     }

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
     pReg->tlbWord0 |= (pReg->ts << __MM_SH_TS);

     /* Set size */
     pReg->tlbWord0 |= (pReg->size << __MM_SH_SIZE);

     /* Set RPN */
     pReg->tlbWord1  = pReg->pAddr;
     pReg->tlbWord1 &= pnMask; 

     /* Set ERPN */
     pReg->tlbWord1 |= pReg->pExtAddr;

     /* Set page attributes */
     pReg->tlbWord2 = (pReg->attr <<  __MM_SH_ATTR);

     /* Set page permission */
     pReg->tlbWord2 |= pReg->perms;

     /* Save lock status in U3 */
     pReg->tlbWord2 |= (pReg->locked << __MM_SH_LOCK);

     return EOK;
}

/************************************************************************
 * This function creates a TLB mapping for a memory area.
 ************************************************************************/

int __mm_map_region(U32 vBase, U32 pBase, U8 erpn, S32 size, U8 perms, U8 attr, U8 flags, U8 pageSize)
{
     MemoryRegion *pReg, *pHead, *pLink;

     /* For now, allow only 1K, 4K and 1M pages  */
     if( size == 0 ||
	 (pageSize != TLB_SIZE_1KB && pageSize != TLB_SIZE_4KB && pageSize != TLB_SIZE_1MB) )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     /* TODO: Add checks on addresses vs existing regions */
     /* TODO: Check address aligned on page size */
     
     pReg = NULL;

     while(size > 0)
     {
	  U32 pSize = 1024 << (pageSize * 2);

	  if( pReg == NULL )
	  {
	       pReg = malloc(sizeof(MemoryRegion));
	       pHead = pReg;
	  }
	  else
	  {
	       pReg->pNext = malloc(sizeof(MemoryRegion));
	       pReg = pReg->pNext;
	  }

	  pReg->vAddr    = vBase;
	  pReg->pAddr    = pBase;
	  pReg->pExtAddr = erpn;
	  pReg->owner    = __MM_GLOBAL_REGION;
	  pReg->perms    = perms;
	  pReg->ts       = TLB_TS_SYSTEM;
	  pReg->attr     = attr;
	  pReg->tid      = __MM_GLOBAL_REGION;
	  pReg->pNext    = NULL;

	  pReg->size = pageSize;

	  size  -= pSize;
	  vBase += pSize;
	  pBase += pSize;

	  if( flags & __MM_LOCK_TLB )
	       __mm_lock_tlb_entry(pReg);

	  __mm_create_tlb_entry(pReg);

	  if( flags & __MM_WRITE_TLB )
	       __mm_load_tlb_entry_reg(pReg);
     }

     /* We want to link our pages into the array in such a way that they are sorted on vAddr */
     if( allocRegions != NULL )
     {
	  if( allocRegions->vAddr > pHead->vAddr )
	  {
	       pReg->pNext = allocRegions;
	       allocRegions = pHead;
	  }
	  else
	  {
	       pLink = allocRegions;
	       while(pLink->pNext)
	       {
		    if( pLink->pNext->vAddr < pHead->vAddr )
			 pLink = pLink->pNext;
		    else
			 break;
	       }
	       pReg->pNext = pLink->pNext;
	       pLink->pNext = pHead;
	  }
     }
     else
	  allocRegions = pHead;

     return EOK;
}

/************************************************************************
 * This function locks a TLB entry, preventing it from being replaced.
 ************************************************************************/
int __mm_lock_tlb_entry(MemoryRegion *pReg)
{
     if( pReg == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     pReg->locked = __MM_TLB_LOCKED;

     return EOK;
}

/************************************************************************
 * This function unlocks a TLB entry, allowing it te be replaced.
 ************************************************************************/
int __mm_unlock_tlb_entry(MemoryRegion *pReg)
{
     if( pReg == NULL )
     {
	  errno = EINVAL;
	  return EERROR;
     }

     pReg->locked = __MM_TLB_UNLOCKED;

     return EOK;
}

/************************************************************************
 * This function locates a memory page based on the virtual address.
 ************************************************************************/
MemoryRegion *__mm_find_region(U32 vAddr)
{
     MemoryRegion *pReg = allocRegions;

     while(pReg != NULL && pReg->vAddr <= vAddr)
     {
	  U32 size = 1024 << (pReg->size * 2);
	  
	  if(pReg->vAddr + size >= vAddr )
	       return pReg;

	  pReg = pReg->pNext;
     }

     /* Did not find region */
     return NULL;
}

/************************************************************************
 * This function loads a TLB entry for the given address.
 ************************************************************************/
int __mm_load_tlb_entry(U32 vAddr)
{
     MemoryRegion *pReg = __mm_find_region(vAddr);
     return __mm_load_tlb_entry_reg(pReg);
}

/************************************************************************
 * This function loads a TLB entry for the given region.
 ************************************************************************/
int __mm_load_tlb_entry_reg(MemoryRegion *pReg)
{
     if( pReg == NULL )
     {
	  /* Could not find the page, this is a fatal error */
	  errno = EFAULT;
	  return EERROR;
     }

     /* Find the page to replace */
     while(loadedPages[currTlbIdx] != NULL)
     {
	  currTlbIdx++;

	  if( currTlbIdx == __MM_MAX_ACTIVE_TLBS )
	       currTlbIdx = 0;
	  if( loadedPages[currTlbIdx]->locked != __MM_TLB_LOCKED )
	       break;
     }

     return __mm_write_tlb_entry(currTlbIdx++, pReg);
}

/******************* EXTERNAL FUNCTIONS *********************/
