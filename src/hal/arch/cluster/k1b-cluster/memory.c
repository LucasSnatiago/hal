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

#include <arch/cluster/k1b-cluster/cores.h>
#include <arch/cluster/k1b-cluster/memory.h>
#include <nanvix/const.h>

/*
 * Addresses should be alined to huge page boundaries.
 */
#if (K1B_HYPER_LOW_BASE_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
#error "bad memory layout"
#endif
#if (K1B_HYPER_HIGH_BASE_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
#error "bad memory layout"
#endif

/**
 * @brief Length of virtual addresses.
 *
 * Number of bits in a virtual address.
 *
 * @author Pedro Henrique Penna
 */
#define K1B_VADDR_LENGTH 32

/**
 * @brief Page Directory length.
 *
 * Number of Page Directory Entries (PDEs) per Page Directory.
 *
 * @author Pedro Henrique Penna
 */
#define K1B_PGDIR_LENGTH (1 << (K1B_VADDR_LENGTH - K1B_PGTAB_SHIFT))

/**
 * @brief Page Table length.
 *
 * Number of Page Table Entries (PTEs) per Page Table.
 *
 * @author Pedro Henrique Penna
 */
#define K1B_PGTAB_LENGTH (1 << (K1B_PGTAB_SHIFT - K1B_PAGE_SHIFT))

/**
 * @brief Root page directory.
 */
PRIVATE struct pde k1b_root_pgdir[K1B_PGDIR_LENGTH] ALIGN(K1B_PAGE_SIZE);

/**
 * @brief Root page table.
 */
PRIVATE struct pte k1b_root_pgtab[K1B_PGTAB_LENGTH] ALIGN(K1B_PAGE_SIZE);

/**
 * Alias to root page directory.
 */
PUBLIC struct pde *root_pgdir = &k1b_root_pgdir[0];

/**
 * Alias to kernel page table.
 */
PUBLIC struct pte *kernel_pgtab = &k1b_root_pgtab[0];

/**
 * Alias to kernel page pool page table.
 */
PUBLIC struct pte *kpool_pgtab = &k1b_root_pgtab[0];

/*
 * Physical Memory Layout
 */
const paddr_t K1B_KERNEL_BASE_PHYS  = K1B_HYPER_LOW_END_PHYS;                             /* Kernel Base        */
const paddr_t K1B_KERNEL_END_PHYS   = (paddr_t)(&_kend);                                  /* Kernel End         */
const paddr_t K1B_KPOOL_BASE_PHYS   = (paddr_t)(&_kend);                                  /* Kernel Pool Base   */
const paddr_t K1B_KPOOL_END_PHYS    = (paddr_t)(&_kend) + K1B_KPOOL_SIZE;                 /* Kernel Pool End    */
const paddr_t K1B_USER_BASE_PHYS    = (paddr_t)(&_kend) + K1B_KPOOL_SIZE;                 /* User Base          */
const paddr_t K1B_USER_END_PHYS     = (paddr_t)(&_kend) + K1B_KPOOL_SIZE + K1B_UMEM_SIZE; /* User End           */
const paddr_t K1B_KSTACK_BASE_PHYS  = (paddr_t)(&_user_stack_start);                      /* Kernel Stack Base  */

/**
 * Virtual Memory Layout
 */
const vaddr_t K1B_KERNEL_BASE_VIRT  = K1B_HYPER_LOW_END_VIRT;                             /* Kernel Base        */
const vaddr_t K1B_KERNEL_END_VIRT   = (vaddr_t)(&_kend);                                  /* Kernel End         */
const vaddr_t K1B_KPOOL_BASE_VIRT   = (vaddr_t)(&_kend);                                  /* Kernel Pool Base   */
const vaddr_t K1B_KPOOL_END_VIRT    = (vaddr_t)(&_kend) + K1B_KPOOL_SIZE;                 /* Kernel Pool End    */
const vaddr_t K1B_USER_BASE_VIRT    = (vaddr_t)(&_kend) + K1B_KPOOL_SIZE;                 /* User Base          */
const vaddr_t K1B_USER_END_VIRT     = (vaddr_t)(&_kend) + K1B_KPOOL_SIZE + K1B_UMEM_SIZE; /* User End           */
const vaddr_t K1B_KSTACK_BASE_VIRT  = (vaddr_t)(&_user_stack_start);                      /* Kernel Stack Base  */

/**
 * @brief Map Hypervisor page frames.
 *
 * The mmu_map_hypervisor() function maps page frames of the
 * hypervisor in the page table pointed to by @p pgtab.
 *
 * @param pgtab Target page table.
 */
PRIVATE void mmu_map_hypervisor(struct pte *pgtab)
{
	/* Fill up Low Hypervisor PTEs. */
	for (vaddr_t vaddr = K1B_HYPER_LOW_BASE_VIRT;
	             vaddr < K1B_HYPER_LOW_END_VIRT;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 0;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}

	/* Fill up High Hypervisor PTEs. */
	for (vaddr_t vaddr = K1B_HYPER_HIGH_BASE_VIRT;
	             vaddr < K1B_HYPER_HIGH_END_VIRT;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 0;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}
}

/**
 * @brief Map Kernel Code and Data pages.
 *
 * The mmu_map_kernel() function maps page frames of the kernel code
 * and data in the page table pointed to by @p pgtab.
 */
PRIVATE void mmu_map_kernel(struct pte *pgtab)
{
	/* Fill up Kernel Code and Data PTEs. */
	for (vaddr_t vaddr = K1B_KERNEL_BASE_PHYS;
	             vaddr < K1B_KERNEL_END_PHYS;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 1;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}
}

/**
 * @brief Map Kernel Page Pool pages.
 *
 * The mmu_map_kpool() function maps page frames of the kernel page
 * pool in the page table pointed to by @p pgtab.
 */
PRIVATE void mmu_map_kpool(struct pte *pgtab)
{
	/* Fill up Kernel Page Pool PTEs. */
	for (vaddr_t vaddr = K1B_KPOOL_BASE_PHYS;
	             vaddr < K1B_KPOOL_END_PHYS;
	             vaddr += K1B_PAGE_SIZE)
	{
		unsigned idx;

		idx = pte_idx_get(vaddr);

		pgtab[idx].present = 1;
		pgtab[idx].writable = 1;
		pgtab[idx].user = 0;
		pgtab[idx].frame = vaddr >> K1B_PAGE_SHIFT;
	}
}

/**
 * @brief Warmups the MMU.
 *
 * The mmu_warmup() function loads the TLB of the underlying core with
 * the initial maping of the system. Overall, it loads Hypervisor the
 * Hypervisor, Kernel and Kernel Page Pool mappings into way one of
 * the architectural TLB, and then it invalidates all entries in way
 * zero.
 *
 * @bug We cannot invalidate entries in way 0 of the TLB.
 */
PRIVATE void mmu_warmup(void)
{
	vaddr_t start, end;

	/* Load Hypervisor entries into the TLB. */
	k1b_tlb_write(
		K1B_HYPER_LOW_BASE_VIRT,
		K1B_HYPER_LOW_BASE_VIRT,
		K1B_HUGE_PAGE_SHIFT,
		1,
		K1B_TLBE_PROT_RWX
	);
	k1b_tlb_write(
		K1B_HYPER_HIGH_BASE_VIRT,
		K1B_HYPER_HIGH_BASE_VIRT,
		K1B_HUGE_PAGE_SHIFT,
		1,
		K1B_TLBE_PROT_RWX
	);

	/* Load Kernel entries into the TLB. */
	start = K1B_KERNEL_BASE_VIRT; end = K1B_KERNEL_END_VIRT;
	for (vaddr_t vaddr = start; vaddr < end; vaddr += K1B_HUGE_PAGE_SIZE)
		k1b_tlb_write(vaddr, vaddr, K1B_HUGE_PAGE_SHIFT, 1, K1B_TLBE_PROT_RWX);

	/* Load Kernel Page Pool entries into the TLB. */
	start = K1B_KPOOL_BASE_VIRT; end = K1B_KPOOL_END_VIRT;
	for (vaddr_t vaddr = start; vaddr < end; vaddr += K1B_HUGE_PAGE_SIZE)
		k1b_tlb_write(vaddr, vaddr, K1B_HUGE_PAGE_SHIFT, 1, K1B_TLBE_PROT_RW);

	/* Invalidate all entries in way 0. */
	start = 0; end = MEMORY_SIZE;
	for (vaddr_t vaddr = start; vaddr < end; vaddr += K1B_PAGE_SIZE)
		k1b_tlb_inval(vaddr, K1B_PAGE_SHIFT, 0);
}

/**
 * @brief Assert memory alignment.
 */
PRIVATE void k1b_mmu_check_alignment(void)
{
	if (K1B_KERNEL_BASE_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
		kpanic("kernel base address misaligned");
	if (K1B_KERNEL_END_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
		kpanic("kernel end address misaligned");
	if (K1B_KPOOL_BASE_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
		kpanic("kernel pool base address misaligned");
	if (K1B_KPOOL_END_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
		kpanic("kernel pool end address misaligned");
	if (K1B_USER_BASE_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
		kpanic("user base address misaligned");
	if (K1B_USER_END_VIRT & (K1B_HUGE_PAGE_SIZE - 1))
		kpanic("user end address misaligned");
}

/**
 * @brief Assert memory layout.
 */
PRIVATE void k1b_mmu_check_layout(void)
{
	size_t kstack_size;

	kstack_size = (vaddr_t)(&_user_stack_start) - (vaddr_t)(&_user_stack_end);
	kstack_size /= K1B_CLUSTER_NUM_CORES;

	if (K1B_KSTACK_BASE_VIRT != (vaddr_t)(&_user_stack_start))
		kpanic("bad kernel stack base address");
	if (kstack_size != K1B_KSTACK_SIZE)
		kpanic("bad kernel stack size");
}

/**
 * The k1b_mmu_setup() function initializes the Memory Management Unit
 * (MMU) of the underlying k1b core.
 */
PUBLIC void k1b_mmu_setup(void)
{
	int coreid;

	coreid = k1b_core_get_id();

	kprintf("[core %d][hal] initializing mmu", coreid);

	/*
	 * Master core builds
	 * root page directory.
	 */
	if (coreid == 0)
	{
		kprintf("[core %d][hal] kernel_base=%x kernel_end=%x",
			coreid,
			K1B_KERNEL_BASE_VIRT,
			K1B_KERNEL_END_VIRT
		);
		kprintf("[core %d][hal]  kpool_base=%x  kpool_end=%x",
			coreid,
			K1B_KPOOL_BASE_VIRT,
			K1B_KPOOL_END_VIRT
		);
		kprintf("[core %d][hal]   user_base=%x   user_end=%x",
			coreid,
			K1B_USER_BASE_VIRT,
			K1B_USER_END_VIRT
		);

		kprintf("[core %d][hal] memsize=%d MB kmem=%d KB kpool=%d KB umem=%d KB",
			coreid,
			MEMORY_SIZE/(1024*1024),
			KMEM_SIZE/1024,
			KPOOL_SIZE/1024,
			UMEM_SIZE/1024
		);

		/* Check for memory layout. */
		k1b_mmu_check_alignment();
		k1b_mmu_check_layout();

		/* Clean root page table. */
		for (int i = 0; i < K1B_PGTAB_LENGTH; i++)
			pte_clear(&k1b_root_pgtab[i]);

		/* Clean root page directory. */
		for (int i = 0; i < K1B_PGDIR_LENGTH; i++)
			pde_clear(&k1b_root_pgdir[i]);

		/* Build root page table. */
		mmu_map_hypervisor(k1b_root_pgtab);
		mmu_map_kernel(k1b_root_pgtab);
		mmu_map_kpool(k1b_root_pgtab);

		/* Build root page directory. */
		k1b_root_pgdir[0].present = 1;
		k1b_root_pgdir[0].writable = 1;
		k1b_root_pgdir[0].user = 0;
		k1b_root_pgdir[0].frame = (vaddr_t)(k1b_root_pgtab) >> K1B_PAGE_SHIFT;
	}

	mmu_warmup();

	k1b_tlb_init();
}
