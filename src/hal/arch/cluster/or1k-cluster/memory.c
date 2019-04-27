/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

#if (defined(__or1k_cluster__))
	#include <arch/cluster/or1k-cluster/memory.h>
	#include <arch/cluster/or1k-cluster/cores.h>
#elif (defined(__optimsoc_cluster__))
	#include <arch/cluster/optimsoc-cluster/memory.h>
	#include <arch/cluster/optimsoc-cluster/cores.h>
#endif

#include <nanvix/hal/core/exception.h>
#include <nanvix/klib.h>
#include <nanvix/const.h>

/**
 * @brief Length of virtual addresses.
 *
 * Number of bits in a virtual address.
 *
 * @author Davidson Francis
 */
#define OR1K_VADDR_LENGTH 32

/**
 * @brief Number of memory regions.
 */
#if (defined(__or1k_cluster__))
	#define OR1K_CLUSTER_MEM_REGIONS 4
#elif (defined(__optimsoc_cluster__))
	#define OR1K_CLUSTER_MEM_REGIONS 3
#endif

/**
 * @brief Page and page table indexes boundaries.
 */
#define MMREGION_PGTABLE_ALIGN_START 0                        /**< mmregion start pgtable aligned. */
#define MMREGION_PGTABLE_ALIGN_END   2                        /**< mmregion end pgtable aligned.   */
#define MMREGION_PG_ALIGN_START      2                        /**< mmregion start page aligned.    */
#define MMREGION_PG_ALIGN_END        OR1K_CLUSTER_MEM_REGIONS /**< mmregion end page aligned.      */

/**
 * @brief Memory region.
 */
struct memory_region
{
	paddr_t pbase;    /**< Base physical address. */
	vaddr_t vbase;    /**< Base virtual address.  */
	paddr_t pend;     /**< End physical address.  */
	vaddr_t vend;     /**< End virtual address.   */
	size_t size;      /**< Size.                  */
	bool writable;    /**< Writable?              */
	bool  executable; /**< Executable?            */
	const char *desc; /**< Description.           */
};

/**
 * @brief Memory layout.
 *
 * @note KERNEL_BASE and KPOOL_BASE *must* be the first two positions,
 * since they are aligned at page table boundaries. The remaining
 * positions left are intended for other devices.
 */
PRIVATE struct memory_region or1k_cluster_mem_layout[OR1K_CLUSTER_MEM_REGIONS] = {
	{ OR1K_CLUSTER_KERNEL_BASE_PHYS, OR1K_CLUSTER_KERNEL_BASE_VIRT,
	  OR1K_CLUSTER_KERNEL_END_PHYS,  OR1K_CLUSTER_KERNEL_END_VIRT,
	  OR1K_CLUSTER_KMEM_SIZE, true, true,  "kernel" },
	
	{ OR1K_CLUSTER_KPOOL_BASE_PHYS, OR1K_CLUSTER_KPOOL_BASE_VIRT,
	  OR1K_CLUSTER_KPOOL_END_PHYS,  OR1K_CLUSTER_KPOOL_END_VIRT,
	  OR1K_CLUSTER_KPOOL_SIZE, true, false, "kpool" },

	{ OR1K_CLUSTER_OMPIC_BASE_PHYS, OR1K_CLUSTER_OMPIC_BASE_VIRT,
	  OR1K_CLUSTER_OMPIC_END_PHYS,  OR1K_CLUSTER_OMPIC_END_VIRT,
	  OR1K_CLUSTER_OMPIC_MEM_SIZE,  true, false, "ompic" },

#if (defined(__or1k_cluster__))
	{ OR1K_CLUSTER_UART_BASE_PHYS, OR1K_CLUSTER_UART_BASE_VIRT,
	  OR1K_CLUSTER_UART_END_PHYS,  OR1K_CLUSTER_UART_END_VIRT,
	  OR1K_CLUSTER_UART_MEM_SIZE,  true, false, "uart" },
#endif
};

/**
 * @brief Root page directory.
 */
PRIVATE struct pde or1k_cluster_root_pgdir[OR1K_PGDIR_LENGTH] ALIGN(PAGE_SIZE);

/**
 * @brief Root page tables.
 */
PRIVATE struct pte or1k_cluster_root_pgtabs[OR1K_CLUSTER_MEM_REGIONS][OR1K_PGTAB_LENGTH] ALIGN(PAGE_SIZE);

/**
 * Alias to root page directory.
 */
PUBLIC struct pde *root_pgdir = &or1k_cluster_root_pgdir[0];

/**
 * Alias to kernel page table.
 */
PUBLIC struct pte *kernel_pgtab = or1k_cluster_root_pgtabs[0];

/**
 * Alias to kernel page pool page table.
 */
PUBLIC struct pte *kpool_pgtab = or1k_cluster_root_pgtabs[1];

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
} or1k_tlb[OR1K_CLUSTER_NUM_CORES];

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
	volatile vaddr_t kcode; /* Kernel text start address. */
	volatile vaddr_t kdata; /* Kernel data start address. */

	kcode = (vaddr_t)&KSTART_CODE;
	kdata = (vaddr_t)&KSTART_DATA;

	/* Kernel address. */
	if (vaddr >= kcode && vaddr < KMEM_SIZE)
	{
		if (vaddr >= kcode && vaddr < kdata)
			return (1);
	}

	/* User address. */
	else
	{
		if (vaddr >= UBASE_VIRT && vaddr < USTACK_VIRT)
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
 * type should be used.
 *
 * @author Davidson Francis
 */
PUBLIC const struct tlbe *or1k_tlb_lookup_vaddr(int tlb_type, vaddr_t vaddr)
{
	const struct tlbe *tlbe; /* TLB Entry Pointer. */
	vaddr_t addr;            /* Aligned address.   */
	int coreid;              /* Core ID.           */

	addr = vaddr & PAGE_MASK;
	coreid = or1k_core_get_id();

	/* Search in ITLB. */
	if (tlb_type == OR1K_TLB_INSTRUCTION)
	{
		for (int i = 0; i < OR1K_TLB_LENGTH; i++)
		{
			tlbe = &or1k_tlb[coreid].itlb[i];

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
			tlbe = &or1k_tlb[coreid].dtlb[i];

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
 * @returns Upon successful completion, the matching TLB
 * entry for the address @p vaddr is returned. If none
 * is found, @p NULL is returned.
 *
 * @author Davidson Francis
 */
PUBLIC const struct tlbe *or1k_tlb_lookup_paddr(int tlb_type, paddr_t paddr)
{
	const struct tlbe *tlbe; /* TLB Entry Pointer. */
	vaddr_t addr;            /* Aligned address.   */
	int coreid;              /* Core ID.           */

	addr = paddr & PAGE_MASK;
	coreid = or1k_core_get_id();

	/* Search in ITLB. */
	if (tlb_type == OR1K_TLB_INSTRUCTION)
	{
		for (int i = 0; i < OR1K_TLB_LENGTH; i++)
		{
			tlbe = &or1k_tlb[coreid].itlb[i];

			/* Found. */
			if (or1k_tlbe_paddr_get(tlbe) == addr)
				return (tlbe);
		}
	}

	/* Search in DTLB. */
	else
	{
		for (int i = 0; i < OR1K_TLB_LENGTH; i++)
		{
			tlbe = &or1k_tlb[coreid].dtlb[i];

			/* Found. */
			if (or1k_tlbe_paddr_get(tlbe) == addr)
				return (tlbe);
		}
	}

	return (NULL);
}

/*============================================================================*
 * or1k_tlb_write()                                                           *
 *============================================================================*/

/**
 * The or1k_tlb_write() function writes an entry into the architectural
 * TLB. If the new entry conflicts to an old one, the old one is
 * overwritten.
 *
 * @note Although the OpenRISC specification states that the TLB can
 * have up to 4-ways, there is no known implementation that uses more
 * than 1-way, i.e: direct mapping. Therefore, this function will use
 * only 1-way at the moment.
 *
 * @author Davidson Francis
 */
PUBLIC int or1k_tlb_write(int tlb_type, vaddr_t vaddr, paddr_t paddr)
{
	volatile vaddr_t kcode; /* Kernel start code. */
	struct tlbe * tlbe;     /* TLB Entry.         */
	unsigned idx;           /* TLB Index.         */
	int coreid;             /* Core ID.           */
	int user;               /* User address flag. */
	int inst;               /* Instruction flag.  */

	idx = (vaddr >> PAGE_SHIFT) & (OR1K_TLB_LENGTH - 1);
	kcode = (vaddr_t)&KSTART_CODE;
	inst = or1k_tlb_check_inst(vaddr);
	coreid = or1k_core_get_id();
	user = 1;

	/*
	 * Check if the virtual address belongs to
	 * kernel or user.
	 */
	if ((vaddr >= kcode && vaddr < KMEM_SIZE))
		user = 0;

	/* Copy to the in-memory Instruction TLB copy. */
	if (tlb_type == OR1K_TLB_INSTRUCTION)
		tlbe =  &or1k_tlb[coreid].itlb[idx];
	
	/* Copy to the in-memory Data TLB copy. */
	else
		tlbe = &or1k_tlb[coreid].dtlb[idx];

	or1k_tlbe_write(tlb_type, user, inst, vaddr, paddr, tlbe);

	return (0);
}

/*============================================================================*
 * or1k_tlb_inval()                                                           *
 *============================================================================*/

/**
 * The or1k_tlb_inval() function invalidates the TLB entry that
 * encodes the virtual address @p vaddr.
 *
 * @author Davidson Francis
 */
PUBLIC int or1k_tlb_inval(int tlb_type, vaddr_t vaddr)
{
	int idx;     /* TLB Index.       */
	int coreid;  /* Core ID.         */

	idx = (vaddr >> PAGE_SHIFT) & (OR1K_TLB_LENGTH - 1);
	coreid = or1k_core_get_id();

	or1k_tlbe_inval(tlb_type, vaddr);

	/*
	 * Invalidates the entry accordingly if
	 * instruction or data.
	 */
	if (tlb_type == OR1K_TLB_INSTRUCTION)
		kmemset(&or1k_tlb[coreid].itlb[idx], 0, OR1K_TLBE_SIZE);

	/* Data. */
	else
		kmemset(&or1k_tlb[coreid].dtlb[idx], 0, OR1K_TLBE_SIZE);

	return (0);
}

/*============================================================================*
 * or1k_tlb_flush()                                                           *
 *============================================================================*/

/**
 * The or1k_tlb_flush() function flushes the changes made to the TLB
 * of the underlying or1k core.
 */
PUBLIC int or1k_tlb_flush(void)
{
	int coreid;       /* Core ID.   */

	coreid = or1k_core_get_id();

	for (int i = 0; i < OR1K_TLB_LENGTH; i++)
	{
		/* Instruction TLB. */
		or1k_tlbe_update(OR1K_TLB_INSTRUCTION, i, &or1k_tlb[coreid].itlb[i]);

		/* Data TLB. */
		or1k_tlbe_update(OR1K_TLB_DATA, i, &or1k_tlb[coreid].dtlb[i]);
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
	// struct tlbe_value tlbev; /* TLB Entry value.                     */
	unsigned dtlbtr;         /* Data TLB Translate Register.         */
	unsigned itlbtr;         /* Instruction TLB Translate Register.  */
	unsigned xtlbmr;         /* Data/Instruction TLB Match Register. */
	int coreid;              /* Core ID.                             */

	dtlbtr = (OR1K_SPR_DTLBTR_CC | OR1K_SPR_DTLBTR_WBC | OR1K_SPR_DTLBTR_SRE
			| OR1K_SPR_DTLBTR_SWE);

	itlbtr = (OR1K_SPR_ITLBTR_CC | OR1K_SPR_ITLBTR_WBC | OR1K_SPR_ITLBTR_SXE);
	xtlbmr = 1;
	coreid = or1k_core_get_id();

	if (!coreid)
		kprintf("[hal] initializing tlb");

	/* Write into DTLB/ITLB. */
	for (int i = 0; i < OR1K_TLB_LENGTH; i++)
	{
		or1k_tlbe_init(
			i,
			dtlbtr,
			itlbtr,
			xtlbmr,
			&or1k_tlb[coreid].dtlb[i],
			&or1k_tlb[coreid].itlb[i]
		);

		dtlbtr += OR1K_PAGE_SIZE;
		itlbtr += OR1K_PAGE_SIZE;
		xtlbmr += OR1K_PAGE_SIZE;
	}
}

/*============================================================================*
 * or1k_do_tlb_fault()                                                        *
 *============================================================================*/

/**
 * @brief Handles a TLB fault.
 *
 * The or1k_do_tlb_fault() function handles a early TLB faults. It
 * checks the current page directory for a virtual-to-physical address
 * mapping, and if it finds one, it writes this mapping to the TLB. If
 * the faulting address is not currently mapped in the current page
 * directory, it panics the kernel.
 *
 * @param excp Exception information.
 * @param ctx  Interrupted execution context.
 *
 * @author Davidson Francis
 * @author Pedro Henrique Penna
 */
PRIVATE void or1k_do_tlb_fault(
	const struct exception *excp,
	const struct context *ctx
)
{
	int tlb;           /* Target TLB.                     */
	paddr_t paddr;     /* Physical address.               */
	vaddr_t vaddr;     /* Faulting address.               */
	struct pte *pte;   /* Working page table table entry. */
	struct pde *pde;   /* Working page directory entry.   */
	struct pte *pgtab; /* Working page table.             */

	/* Get page address of faulting address. */
	vaddr = or1k_excp_get_addr(excp);
	vaddr &= OR1K_PAGE_MASK;

	/* Lookup PDE. */
	pde = pde_get(root_pgdir, vaddr);
	if (!pde_is_present(pde))
	{
		or1k_context_dump(ctx);
		kpanic("[hal] page fault at %x", exception_get_addr(excp));
	}

	/* Lookup PTE. */
	pgtab = (struct pte *)(pde_frame_get(pde) << OR1K_PAGE_SHIFT);
	pte = pte_get(pgtab, vaddr);
	if (!pte_is_present(pte))
	{
		or1k_context_dump(ctx);
		kpanic("[hal] page fault at %x", exception_get_addr(excp));
	}

	/* Writing mapping to TLB. */
	paddr = pte_frame_get(pte) << OR1K_PAGE_SHIFT;
	tlb = (excp->num == OR1K_EXCP_ITLB_FAULT) ?
		OR1K_TLB_INSTRUCTION : OR1K_TLB_DATA;
	if (or1k_tlb_write(tlb, vaddr, paddr) < 0)
		kpanic("[hal] cannot write to tlb");
}

/*============================================================================*
 * or1k_enable_mmu()                                                          *
 *============================================================================*/

/**
 * The or1k_mmu_enable() function enables the MMU of the underlying
 * or1k core.
 */
PUBLIC void or1k_enable_mmu(void)
{
	or1k_mtspr(OR1K_SPR_SR, or1k_mfspr(OR1K_SPR_SR)
		| OR1K_SPR_SR_DME | OR1K_SPR_SR_IME);
}

/*============================================================================*
 * or1k_mmu_setup()                                                           *
 *============================================================================*/

/**
 * The or1k_mmu_setup() function initializes the Memory Management Unit
 * (MMU) of the underlying or1k core.
 */
PUBLIC void or1k_mmu_setup(void)
{
	/* TLB Handler. */
	exception_register(EXCEPTION_DTLB_FAULT, or1k_do_tlb_fault);
	exception_register(EXCEPTION_ITLB_FAULT, or1k_do_tlb_fault);

	/* Initial TLB. */
	or1k_tlb_init();

	/* Enable MMU. */
	or1k_enable_mmu();
}

/*============================================================================*
 * or1k_cluster_mem_info()                                                    *
 *============================================================================*/

/**
 * @brief Prints information about memory layout.
 *
 * The or1k_cluster_mem_info() prints information about the virtual
 * memory layout.
 *
 * @author Davidson Francis
 */
PRIVATE void or1k_cluster_mem_info(void)
{
	int i; /* Loop index. */

	for (i = 0; i < OR1K_CLUSTER_MEM_REGIONS; i++)
	{
		kprintf("[hal] %s_base=%x %s_end=%x",
			or1k_cluster_mem_layout[i].desc,
			or1k_cluster_mem_layout[i].vbase,
			or1k_cluster_mem_layout[i].desc,
			or1k_cluster_mem_layout[i].vend
		);
	}

	kprintf("[hal] user_base=%x   user_end=%x",
		OR1K_CLUSTER_USER_BASE_VIRT,
		OR1K_CLUSTER_USER_END_VIRT
	);

	kprintf("[hal] memsize=%d MB kmem=%d KB kpool=%d KB umem=%d KB",
		OR1K_CLUSTER_MEM_SIZE/MB,
		OR1K_CLUSTER_KMEM_SIZE/KB,
		OR1K_CLUSTER_KPOOL_SIZE/KB,
		OR1K_CLUSTER_UMEM_SIZE/KB
	);
}

/*============================================================================*
 * or1k_cluster_mem_check_align()                                             *
 *============================================================================*/

/**
 * @brief Asserts the memory alignment.
 *
 * @todo TODO provide a detailed description for this function.
 *
 * @author Davidson Francis
 */
PRIVATE void or1k_cluster_mem_check_align(void)
{
	int i; /* Loop index. */

	/* These should be aligned at page boundaries. */
	for (i = MMREGION_PG_ALIGN_START; i < MMREGION_PG_ALIGN_END; i++)
	{
		if (or1k_cluster_mem_layout[i].vbase & (OR1K_PAGE_SIZE - 1))
			kpanic("%s base address misaligned", or1k_cluster_mem_layout[i].desc);
		if (or1k_cluster_mem_layout[i].vend  & (OR1K_PAGE_SIZE - 1))
			kpanic("%s end address misaligned", or1k_cluster_mem_layout[i].desc);
	}

	/* These should be aligned at page table boundaries. */
	for (i = MMREGION_PGTABLE_ALIGN_START; i < MMREGION_PGTABLE_ALIGN_END; i++)
	{
		if (or1k_cluster_mem_layout[i].vbase & (OR1K_PGTAB_SIZE - 1))
			kpanic("%s base address misaligned", or1k_cluster_mem_layout[i].desc);
		if (or1k_cluster_mem_layout[i].vend  & (OR1K_PGTAB_SIZE - 1))
			kpanic("%s end address misaligned", or1k_cluster_mem_layout[i].desc);	
	}

	if (OR1K_CLUSTER_USER_BASE_VIRT & (OR1K_PGTAB_SIZE - 1))
		kpanic("user base address misaligned");
	if (OR1K_CLUSTER_USER_END_VIRT & (OR1K_PGTAB_SIZE - 1))
		kpanic("user end address misaligned");
}

/*============================================================================*
 * or1k_cluster_mem_check_layout()                                            *
 *============================================================================*/

/**
 * @brief Asserts the memory layout.
 *
 * @todo TODO provide a detailed description for this function.
 *
 * @author Davidson Francis
 */
PRIVATE void or1k_cluster_mem_check_layout(void)
{
	int i; /* Loop index. */

	/*
	 * These should be identity mapped, becasuse the underlying
	 * hypervisor runs with virtual memory disabled.
	 */
	for (i = 0; i < OR1K_CLUSTER_MEM_REGIONS; i++)
	{
		if (or1k_cluster_mem_layout[i].vbase != or1k_cluster_mem_layout[i].pbase)
		{
			kpanic("%s base address is not identity mapped",
				or1k_cluster_mem_layout[i].desc
			);
		}
		if (or1k_cluster_mem_layout[i].vend != or1k_cluster_mem_layout[i].pend)
		{
			kpanic("%s end address is not identity mapped",
				or1k_cluster_mem_layout[i].desc
			);
		}
	}
}

/*============================================================================*
 * or1k_cluster_mem_map()                                                     *
 *============================================================================*/

/**
 * @brief Builds the memory layout.
 *
 * @todo TODO provide a detailed description for this function.
 *
 * @author Davidson Francis
 */
PRIVATE void or1k_cluster_mem_map(void)
{
	/* Clean root page directory. */
	for (int i = 0; i < OR1K_PGDIR_LENGTH; i++)
		pde_clear(&or1k_cluster_root_pgdir[i]);

	/* Build root address space. */
	for (int i = 0; i < OR1K_CLUSTER_MEM_REGIONS; i++)
	{
		paddr_t j;
		vaddr_t k;
		paddr_t pbase = or1k_cluster_mem_layout[i].pbase;
		vaddr_t vbase = or1k_cluster_mem_layout[i].vbase;
		size_t size = or1k_cluster_mem_layout[i].size;
		int w = or1k_cluster_mem_layout[i].writable;
		int x = or1k_cluster_mem_layout[i].executable;

		/* Map underlying pages. */
		for (j = pbase, k = vbase;
			 k < (pbase + size);
			 j += OR1K_PAGE_SIZE, k += OR1K_PAGE_SIZE)
		{
			or1k_page_map(or1k_cluster_root_pgtabs[i], j, k, w, x);
		}

		/* Map underlying page table. */
		or1k_pgtab_map(
				or1k_cluster_root_pgdir,
				OR1K_PADDR(or1k_cluster_root_pgtabs[i]),
				vbase
		);
	}
}

/*============================================================================*
 * or1k_cluster_mem_setup()                                                   *
 *============================================================================*/

/**
 * The or1k_cluster_mem_setup() function initializes the Memory
 * Interface of the underlying OpenRISC Cluster.
 *
 * @author Davidson Francis
 */
PUBLIC void or1k_cluster_mem_setup(void)
{
	int coreid;

	coreid = or1k_core_get_id();

	kprintf("[hal] initializing memory layout...");

	/* Master core builds root virtual address space. */
	if (coreid == OR1K_CLUSTER_COREID_MASTER)
	{
		or1k_cluster_mem_info();

		/* Check for memory layout. */
		or1k_cluster_mem_check_layout();
		or1k_cluster_mem_check_align();

		or1k_cluster_mem_map();
	}

	/*
	 * TODO: slave cores should warmup the TLB.
	 */
}
