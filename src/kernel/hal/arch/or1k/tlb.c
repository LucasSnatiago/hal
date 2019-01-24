/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis     <davidsondfgl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <arch/or1k/core.h>
#include <arch/or1k/tlb.h>
#include <nanvix/mm.h>
#include <nanvix/const.h>

/**
 * @brief TLB
 *
 * Architectural Translation Lookaside Buffer (TLB).
 *
 * @author Davidson Francis
 */
PRIVATE struct
{
	/**
	 * @brief Data TLB.
	 */
	struct tlbe dtlb[OR1K_TLB_LENGTH];

	/**
	 * @brief Instruction TLB.
	 */
	struct tlbe itlb[OR1K_TLB_LENGTH];
} tlb;

/**
 * @brief TLB Entry Value
 * 
 * Converts between a structure value and a single value
 * writable into xTLBMR and xTLBTR.
 */
struct tlbe_value
{
	union
	{
		/* TLB Entry. */
		struct tlbe tlbe;
		
		/* TLB Value. */
		uint64_t value;
	} u;
} __attribute__((packed));

/**
 * TLBE xTLBMR
 */
#define OR1K_TLBE_xTLBMR(x) ((x) >> 32)

/**
 * TLBE xTLBTR
 */
#define OR1K_TLBE_xTLBTR(x) ((x) & 0xFFFFFFFF)

/*============================================================================*
 * or1k_tlb_check_inst()                                                      *
 *============================================================================*/

/**
 * The or1k_tlb_check_inst() function checks by a given virtual address
 * @p vaddr if belongs to code or data.
 *
 * @param vaddr Virtual address to be checked.
 *
 * @returns Returns 1 if the virtual adress belongs to code,
 * and 0 otherwise.
 *
 * @author Davidson Francis
 */
PRIVATE int or1k_tlb_check_inst(vaddr_t vaddr)
{
	/* Kernel address. */
	if (vaddr >= KSTART_CODE && vaddr < KMEM_SIZE)
	{
		if (vaddr >= KSTART_CODE && vaddr < KSTART_DATA)
			return (1);
	}

	/* User address. */
	else
	{
		if (vaddr >= UBASE_VIRT && vaddr < USTACK_ADDR)
			return (1);
	}

	return (0);
}

/*============================================================================*
 * or1k_tlb_lookup_vaddr()                                                    *
 *============================================================================*/

/**
 * The or1k_tlb_lookup_vaddr() function searches the architectural TLB
 * for an entry that matches the virtual address @p vaddr.
 *
 * @param vaddr Address to be queried.
 *
 * @returns Upon successful completion, the matching TLB
 * entry for the address @p vaddr is returned. If none
 * is found, @p NULL is returned.
 *
 * @author Davidson Francis
 */
PUBLIC const struct tlbe *or1k_tlb_lookup_vaddr(vaddr_t vaddr)
{
	const struct tlbe *tlbe; /* TLB Entry Pointer. */
	vaddr_t addr;            /* Aligned address.   */
	
	addr = vaddr & PAGE_MASK;

	/* Search in ITLB. */
	if (or1k_tlb_check_inst(vaddr))
	{
		for (int i = 0; i < OR1K_TLB_LENGTH; i++)
		{
			tlbe = &tlb.itlb[i];

			/* Found. */
			if (or1k_tlbe_vaddr_get(tlbe) == addr)
				return (tlbe);
		}
	}

	/* Search in DTLB. */
	else
	{
		for (int i = 0; i < OR1K_TLB_LENGTH; i++)
		{
			tlbe = &tlb.dtlb[i];

			/* Found. */
			if (or1k_tlbe_vaddr_get(tlbe) == addr)
				return (tlbe);
		}
	}

	return (NULL);
}

/*============================================================================*
 * or1k_tlb_lookup_paddr()                                                    *
 *============================================================================*/

/**
 * The or1k_tlb_lookup_paddr() function searches the architectural TLB
 * for an entry that matches the physical address @p paddr.
 *
 * @param vaddr Address to be queried.
 *
 * @returns Upon successful completion, the matching TLB
 * entry for the address @p vaddr is returned. If none
 * is found, @p NULL is returned.
 *
 * @note It's important to note that, since this function uses the
 * physical address instead of virtual, it's not possible to tell
 * with certainty that the address belongs to instruction or data.
 * Taken this into account, or1k_tlb_lookup_paddr will check both xTLB
 * and will return the first entry that the physical address match.
 * The case when the function returns the wrong entry only occurs if
 * the page is both instruction and data, which is kind rare.
 *
 * @author Davidson Francis
 */
PUBLIC const struct tlbe *or1k_tlb_lookup_paddr(paddr_t paddr)
{
	const struct tlbe *tlbe_i;  /* ITLB Entry Pointer. */
	const struct tlbe *tlbe_d;  /* DTLB Entry Pointer. */
	vaddr_t addr;               /* Aligned address.   */
	
	addr = paddr & PAGE_MASK;

	/* Search in TLB. */
	for (int i = 0; i < OR1K_TLB_LENGTH; i++)
	{
		tlbe_i = &tlb.itlb[i];
		tlbe_d = &tlb.dtlb[i];

		/* Found in ITLB. */
		if (or1k_tlbe_paddr_get(tlbe_i) == addr)
			return (tlbe_i);

		/* Found in DTLB. */
		if (or1k_tlbe_paddr_get(tlbe_d) == addr)
			return (tlbe_d);
	}

	return (NULL);
}

/*============================================================================*
 * or1k_tlb_write()                                                           *
 *============================================================================*/

/**
 * THe or1k_tlb_write() function writes an entry into the architectural
 * TLB. If the new entry conflicts to an old one, the old one is
 * overwritten.
 *
 * @note This function tries to guess which TLB (Data or Instruction)
 * should be used by checking the virtual address. Although this works
 * for the most cases, sometimes this can led to wrong behaviour, for
 * instance, if the TLB miss was triggered by a read from a virtual
 * address belonging to a text segment: in this case a DTLB-Miss will
 * be triggered, but, since the virtual address is relative to a code,
 * only the ITLB will be write.
 *
 * @author Davidson Francis
 */
PUBLIC int or1k_tlb_write(vaddr_t vaddr, paddr_t paddr)
{
	struct tlbe_value tlbev; /* TLB Entry value.   */
	struct tlbe tlbe;        /* TLB Entry.         */
	unsigned idx;            /* TLB Index.         */
	unsigned user;           /* User address flag. */
	unsigned inst;           /* Instruction flag.  */

	user = 1;
	/*
	 * Check if the virtual address belongs to
	 * kernel or user.
	 */
	if (vaddr >= KSTART_CODE && vaddr < KMEM_SIZE)
		user = 0;

	kmemset(&tlbe, 0, OR1K_TLBE_SIZE);

	/* 
	 * Check if the virtual address belongs to
	 * instruction or data.
	 */
	if ((inst = or1k_tlb_check_inst(vaddr)))
	{
		if (!user)
			tlbe.perms = OR1K_ITLBE_SXE;
		else
			tlbe.perms = OR1K_ITLBE_UXE;
	}

	/* Data. */
	else
	{
		if (!user)
			tlbe.perms = OR1K_DTLBE_SRE | OR1K_DTLBE_SwE;
		else
		{
			tlbe.perms = OR1K_DTLBE_SRE | OR1K_DTLBE_SwE |
				OR1K_DTLBE_URE | OR1K_DTLBE_UWE;
		}
	}

	/* Remaining fields. */
	tlbe.vpn = vaddr >> PAGE_SHIFT;
	tlbe.lru = 0;
	tlbe.cid = 0;
	tlbe.pl = OR1K_TLBE_PL2;
	tlbe.valid = OR1K_TLBE_VALID;

	tlbe.ppn = paddr >> PAGE_SHIFT;
	tlbe.dirty = !OR1K_TLBE_DIRTY;
	tlbe.accessed = !OR1K_TLBE_ACCESSED;
	tlbe.wom = OR1K_TLBE_MEMORY_MODEL_STRONG; 
	tlbe.wbc = OR1K_TLBE_CACHE_POLICY_WRBACK;
	tlbe.ci = !OR1K_TLBE_CACHE_INHIBIT;
	tlbe.cc = OR1K_TLBE_CACHE_COHERENCY;

	/* TLB index. */
	idx = (vaddr >> PAGE_SHIFT) & (OR1K_TLB_LENGTH - 1);
	tlbev.u.tlbe = tlbe;
	
	if (inst)
	{
		/* Copy to the in-memory TLB copy. */
		kmemcpy(&tlb.itlb[idx], &tlbe, OR1K_TLBE_SIZE);

		/* Copy to HW TLB. */
		or1k_mtspr(OR1K_SPR_ITLBTR_BASE(0) | idx, OR1K_TLBE_xTLBTR(tlbev.u.value));
		or1k_mtspr(OR1K_SPR_ITLBMR_BASE(0) | idx, OR1K_TLBE_xTLBMR(tlbev.u.value));
	}
	else
	{
		/* Copy to the in-memory TLB copy. */
		kmemcpy(&tlb.dtlb[idx], &tlbe, OR1K_TLBE_SIZE);

		/* Copy to HW TLB. */
		or1k_mtspr(OR1K_SPR_DTLBTR_BASE(0) | idx, OR1K_TLBE_xTLBTR(tlbev.u.value));
		or1k_mtspr(OR1K_SPR_DTLBMR_BASE(0) | idx, OR1K_TLBE_xTLBMR(tlbev.u.value));
	}

	return (0);
}

/**
 * The or1k_tlb_inval() function invalidates the TLB entry that
 * encodes the virtual address @p vaddr.
 *
 * @author Davidson Francis
 */
PUBLIC int or1k_tlb_inval(vaddr_t vaddr)
{
	struct tlbe_value tlbev; /* TLB Entry value. */
	int idx;                 /* TLB Index.       */

	idx = (vaddr >> PAGE_SHIFT) & (OR1K_TLB_LENGTH - 1);
	tlbev.u.value = 0;

	/*
	 * Invalidates the entry accordingly if
	 * instruction or data.
	 */
	if (or1k_tlb_check_inst(vaddr))
	{
		kmemcpy(&tlb.itlb[idx], &tlbev.u.tlbe, OR1K_TLBE_SIZE);
		or1k_mtspr(OR1K_SPR_ITLBMR_BASE(0) | idx, 0);
	}

	/* Data. */
	else
	{
		kmemcpy(&tlb.dtlb[idx], &tlbev.u.tlbe, OR1K_TLBE_SIZE);
		or1k_mtspr(OR1K_SPR_DTLBMR_BASE(0) | idx, 0);
	}

	return (0);
}

/*============================================================================*
 * or1k_tlb_flush()                                                           *
 *============================================================================*/

/**
 * @brief Flushes changes in the TLB.
 *
 * The or1k_tlb_flush() function flushes the changes made to the
 * TLB of the underlying or1k core.
 *
 * @returns This function always returns zero.
 */
PUBLIC int or1k_tlb_flush(void)
{
	unsigned nsets;
	unsigned dtlbmr_base;
	unsigned itlbmr_base;

	nsets = (1 << ((or1k_mfspr(OR1K_SPR_DMMUCFGR) & OR1K_SPR_DMMUCFGR_NTS)
			>> OR1K_SPR_DMMUCFGR_NTS_OFF));
		
	dtlbmr_base = OR1K_SPR_DTLBMR_BASE(0);
	itlbmr_base = OR1K_SPR_ITLBMR_BASE(0);

	for (unsigned i = 0; i < nsets; i++)
	{
		or1k_mtspr(dtlbmr_base, 0);
		or1k_mtspr(itlbmr_base, 0);
		dtlbmr_base++;
		itlbmr_base++;
	}

	return (0);
}

/*============================================================================*
 * or1k_tlb_init()                                                            *
 *============================================================================*/

/**
 * The or1k_tlb_init() initializes the architectural TLB.
 */
PUBLIC void or1k_tlb_init(void)
{
	struct tlbe_value tlbev; /* TLB Entry value.                     */
	unsigned dtlbtr;         /* Data TLB Translate Register.         */
	unsigned itlbtr;         /* Instruction TLB Translate Register.  */
	unsigned xtlbmr;         /* Data/Instruction TLB Match Register. */

	dtlbtr = (OR1K_SPR_DTLBTR_CC | OR1K_SPR_DTLBTR_WBC | OR1K_SPR_DTLBTR_SRE
			| OR1K_SPR_DTLBTR_SWE);
	
	itlbtr = (OR1K_SPR_ITLBTR_CC | OR1K_SPR_ITLBTR_WBC | OR1K_SPR_ITLBTR_SXE);
	xtlbmr = 1;

	kprintf("[hal] initializing tlb");

	/* Write into DTLB/ITLB. */
	for (int i = 0; i < OR1K_TLB_LENGTH; i++)
	{
		or1k_mtspr(OR1K_SPR_DTLBTR_BASE(0) | i, dtlbtr);
		or1k_mtspr(OR1K_SPR_ITLBTR_BASE(0) | i, itlbtr);
		or1k_mtspr(OR1K_SPR_DTLBMR_BASE(0) | i, xtlbmr);
		or1k_mtspr(OR1K_SPR_ITLBMR_BASE(0) | i, xtlbmr);

		tlbev.u.value = ((uint64_t)xtlbmr << 32) | dtlbtr;
		kmemcpy(&tlb.dtlb[i], &tlbev.u.tlbe, OR1K_TLBE_SIZE);

		tlbev.u.value = ((uint64_t)xtlbmr << 32) | itlbtr;
		kmemcpy(&tlb.itlb[i], &tlbev.u.tlbe, OR1K_TLBE_SIZE);

		dtlbtr += OR1K_PAGE_SIZE;
		itlbtr += OR1K_PAGE_SIZE;
		xtlbmr += OR1K_PAGE_SIZE;
	}
}
