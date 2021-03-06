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

#ifndef _ppcos_mm_h_
#define _ppcos_mm_h_

#include "krntypes.h"
#include "arch/ppc440.h"

/* Value defines */

/* TLB page sizes, see page 110 of the PPC440 UM */
#define TLB_SIZE_1KB      0
#define TLB_SIZE_4KB      1
#define TLB_SIZE_16KB     2
#define TLB_SIZE_64KB     3
#define TLB_SIZE_256KB    4
#define TLB_SIZE_1MB      5
#define TLB_SIZE_16MB     7
#define TLB_SIZE_256MB    9

/* TLB page permissions, see page 112 of the PPC440 UM */
#define TLB_PERM_SR       1  /* Supervisor Read Access */
#define TLB_PERM_SW       2  /* Supervisor Write Access */
#define TLB_PERM_SX       4  /* Supervisor Execute Access */
#define TLB_PERM_UR       8  /* User Read Access */
#define TLB_PERM_UW       16 /* User Write Access */
#define TLB_PERM_UX       32 /* User Execute Access */

/* TLB storage attributes, see page 114-116 */
#define TLB_ATTR_NONE          0
#define TLB_ATTR_ENDIAN        1  /* If set, the page uses little-endian access */
#define TLB_ATTR_GUARDED       2  /* Page is guarded against speculative access */
#define TLB_ATTR_MC            4  /* Memory Coherence Requires (only for multi core) */
#define TLB_ATTR_CACHE_INHIBIT 8  /* Chaching is inhibited for the page */
#define TLB_ATTR_WRITE_THROUGH 16 /* Page is cache write-through */

/* TLB valid bit */
#define TLB_VALID      0x200

/* Translation Address Space */
#define TLB_TS_SYSTEM    0
#define TLB_TS_USER      1

/* Owner for global regions */
#define __MM_GLOBAL_REGION 0

/* TLB index size on PPC440 */
#define __MM_MAX_ACTIVE_TLBS NUM_TLB_ENTRIES

/* Lock defines */
#define __MM_TLB_UNLOCKED 0
#define __MM_TLB_LOCKED   1

/* Loaded defines */
#define __MM_TLB_UNLOADED 0
#define __MM_TLB_LOADED   1

/* Flags for mm_map_region */
#define __MM_LOCK_TLB  1 /* Prevent TLB entries from being overwritten */
#define __MM_WRITE_TLB 2 /* Commit TLB entry to UTLB immediatley */

/* Shift offsets */
#define __MM_SH_SIZE 4
#define __MM_SH_TS   9
#define __MM_SH_ATTR 7
#define __MM_SH_LOCK 12

/* Data structures */

typedef struct _MemoryRegion {
     U32 vAddr;     /* Virtual address of region */
     U32 pAddr;     /* Physical address of region */
     U8  pExtAddr;  /* ERPN protion of physical address */
     U32 owner;     /* PID of owning process, or 0 for global */
     U8  size;      /* Size of region, as defined in the PPC440 UM: 4^(size)kb,
		       where size € {0, 1, 2, 3, 4, 5, 7, 9} */
     U8 perms;      /* Permissions for the memory region */
     U8 ts;         /* Translation Address Space, 0 for system pages, 1 for user pages */
     U8 attr;       /* Attributes for the region */

     U32 tlbWord0;   /* Word 0 of the TLB entry */
     U32 tlbWord1;   /* Word 1 of the TLB entry */
     U32 tlbWord2;   /* Word 2 of the TLB entry */
     U8  tid;        /* Translation ID for Word 0 */

     U8 locked;      /* Specifies if the TLB can be replaced or not */
     U8 loaded;      /* Specifies if the region is currently loaded in the TLB */

     struct _MemoryRegion *pNext;
} MemoryRegion;

/* POSIX function used by malloc() in newlib */

/* Kernel internal interface */

void          __mm_init(); 
void          __mm_invalidate_utlb(bool preserveLocked);
int           __mm_create_tlb_entry(MemoryRegion *pReg);
int           __mm_write_tlb_entry(U8 tlbIdx, MemoryRegion *pReg);
void          __mm_invalidate_stlb();
int           __mm_map_region(U32 vBase, U32 pBase, U8 erpn, S32 size, U8 perms, U8 attr, U8 flags, U8 pageSize);
int           __mm_lock_tlb_entry(MemoryRegion *pReg);
int           __mm_unlock_tlb_entry(MemoryRegion *pReg);
MemoryRegion *__mm_find_region(U32 vAddr);
int           __mm_load_tlb_entry(U32 vAddr);
int           __mm_load_tlb_entry_reg(MemoryRegion *pReg);

/* External interface */
int mm_allocate_region(U32 size, U32 base);

#endif  /* _ppcos_mm_h_ */
