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

#ifndef ARCH_CORE_K1B_TLB_H_
#define ARCH_CORE_K1B_TLB_H_

/**
 * @addtogroup k1b-core-tlb TLB
 * @ingroup k1b-core
 *
 * @brief Translation Lookaside Buffer
 */
/**@{*/

	#include <arch/core/k1b/mmu.h>
	#include <nanvix/klib.h>
	#include <mOS_vcore_u.h>
	#include <mOS_segment_manager_u.h>
	#include <errno.h>

	/**
	 * @name TLB Types
	 */
	/**@{*/
	#define K1B_TLB_INSTRUCTION 0 /**< Instruction TLB */
	#define K1B_TLB_DATA        1 /**< Data TLB        */
	/**@}*/

	/**
	 * @brief Length of Locked TLB (number of entries).
	 */
	#ifdef __k1io__
		#define K1B_LTLB_LENGTH 16
	#else
		#define K1B_LTLB_LENGTH 8
	#endif

	/**
	 * @brief Length of Join TLB (number of entries).
	 */
	#define K1B_JTLB_LENGTH 128

	/**
	 * @brief Offset of JTLB in the TLB (number of entries).
	 */
	#define K1B_JTLB_OFFSET 0

	/**
	 * @brief Offset of LTLB in the TLB (number of entries).
	 */
	#define K1B_LTLB_OFFSET 256

	/**
	 * @brief Length of architectural TLB (number of entries).
	 */
	#define K1B_TLB_LENGTH (K1B_JTLB_LENGTH + K1B_LTLB_LENGTH)

	/**
	 * @brief TLB entry size (in bytes).
	 */
	#define K1B_TLBE_SIZE 8

	/**
	 * @name Data TLBE Cache Policies
	 */
	/**@{*/
	#define K1B_DTLBE_CACHE_POLICY_DEVICE    0 /**< Device Access */
	#define K1B_DTLBE_CACHE_POLICY_BYPASS    1 /**< Cache Bypass  */
	#define K1B_DTLBE_CACHE_POLICY_WRTHROUGH 2 /**< Write Through */
	/**@*/

	/**
	 * @name Instruction TLBE Cache Policies
	 */
	/**@{*/
	#define K1B_ITLBE_CACHE_POLICY_BYPASS 0 /**< Cache Bypass  */
	#define K1B_ITLBE_CACHE_POLICY_ENABLE 2 /**< Cache Enable  */
	/**@}*/

	/**
	 * @brief TLB Entry Protection Attributes
	 *
	 * @todo When running in bare-metal (without Hypervisor) we should
	 * revise this.
	 */
	/**@{*/
	#define K1B_TLBE_PROT_R     5 /**< Read-Only               */
	#define K1B_TLBE_PROT_RW    9 /**< Read and Write          */
	#define K1B_TLBE_PROT_RX   11 /**< Read and Execute        */
	#define K1B_TLBE_PROT_RWX  13 /**< Read, Write and Execute */
	/**@}*/

	/**
	 * @name TLB Entry Status
	 */
	/**@{*/
	#define K1B_TLBE_STATUS_INVALID   0 /**< Invalid         */
	#define K1B_TLBE_STATUS_PRESENT   1 /**< Present         */
	#define K1B_TLBE_STATUS_MODIFIED  2 /**< Modified        */
	#define K1B_TLBE_STATUS_AMODIFIED 3 /**< Atomic Modified */
	/**@}*/

/**
 * @cond k1b
 */

	/**
	 * @brief TLB entry.
	 */
	struct tlbe
	{
		unsigned status       :  2; /**< Entry Status (ES)          */
		unsigned cache_policy :  2; /**< Cache Policy (CP)          */
		unsigned protection   :  4; /**< Protection Attributes (PA) */
		unsigned addr_ext     :  4; /**< Address  Extension (AE)    */
		unsigned frame        : 20; /**< Frame Number (FN)          */
		unsigned addrspace    :  9; /**< Address Space Number (ANS) */
		unsigned              :  1; /**< Reserved                   */
		unsigned global       :  1; /**< Global Page Indicator (G)  */
		unsigned size         :  1; /**< Page Size (S)              */
		unsigned page         : 20; /**< Page Number (PN)           */
	} __attribute__((packed));

/**@endcond*/

	/**
	 * @brief Gets the virtual address of a page.
	 *
	 * @param tlbe Target TLB entry.
	 *
	 * The k1b_tlbe_vaddr() function gets the virtual address of a
	 * page encoded in the TLB entry pointed to by @p tlbe.
	 *
	 * @return The virtual address of a page enconded in the TLB entry
	 * pointed to by @p tlbe.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline vaddr_t k1b_tlbe_vaddr_get(const struct tlbe *tlbe)
	{
		unsigned pns = ((tlbe->page << 1) | tlbe->size);
		unsigned pn = pns >> (__builtin_k1_ctz(pns) + 1);
		return (pn << (__builtin_k1_ctz(pns) + 12));
	}

	/**
	 * @brief Gets the physical address of a page.
	 *
	 * @param tlbe Target TLB entry.
	 *
	 * The k1b_tlbe_paddr() function gets the physical address of a
	 * page frame encoded in the TLB entry pointed to by @p tlbe.
	 *
	 * @return The physical address of a page encoded in the TLB entry
	 * pointed to by @p tlbe.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline paddr_t k1b_tlbe_paddr_get(const struct tlbe *tlbe)
	{
		return (tlbe->frame << 12);
	}

	/**
	 * @brief Gets the size of a page.
	 *
	 * @param tlbe Target TLB entry.
	 *
	 * The k1b_tlbe_pgsize() function gets the size (in bytes) of a
	 * page enconded in the TLB entry pointed to by @p tlbe.
	 *
	 * @returns The size (in bytes) of the page encoded in the TLB
	 * entry pointed to by @p tlbe.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline size_t k1b_tlbe_pgsize_get(const struct tlbe *tlbe)
	{
		unsigned pns = ((tlbe->page << 1) | tlbe->size);
		return (1 << (__builtin_k1_ctz(pns) + 12));
	}

	/**
	 * @brief Reads a TLB entry.
	 *
	 * The k1b_tlbe_read() function reads the TLB entry @p idx and
	 * saves it in the location pointed to by @p tlbe.
	 *
	 * @param tlbe Target store location.
	 * @param idx  Index of target TLB entry.
	 *
	 * @returns Upon successful completion, non zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 *
	 * @author Pedro Henrique Penna
	 */
	static inline int k1b_tlbe_read(struct tlbe *tlbe, int idx)
	{
		__k1_tlb_entry_t _tlbe;
		_tlbe = mOS_readtlb(idx);
		kmemcpy(tlbe, &_tlbe, K1B_TLBE_SIZE);
		return (0);
	}

	/**
	 * @brief Writes a TLB entry.
	 *
	 * @param vaddr      Target virtual address.
	 * @param paddr      Target physical address.
	 * @param shift      Page shift.
	 * @param way        Target set-associative way.
	 * @param protection Protection attributes.
	 */
	extern int k1b_tlb_write(
			vaddr_t vaddr,
			paddr_t paddr,
			unsigned shift,
			unsigned way,
			unsigned protection
	);

	/**
	 * @brief Invalidates a TLB entry.
	 *
	 * @param vaddr Target virtual address.
	 * @param shift Page shift.
	 * @param way   Target set-associative way.
	 */
	extern int k1b_tlb_inval(vaddr_t vaddr, unsigned shift, unsigned way);

	/**
	 * @brief Dumps a TLB entry.
	 *
	 * @param idx Index of target entry in the TLB.
	 */
	extern void k1b_tlbe_dump(int idx);

	/**
	 * @brief Lookups a TLB entry by virtual address.
	 *
	 * @param vaddr Target virtual address.
	 *
	 * @returns Upon successful completion, a pointer to the TLB entry
	 * that matches the virtual address @p vaddr is returned. If no
	 * entry that meets this criteria is found, @p NULL is returned.
	 */
	extern const struct tlbe *k1b_tlb_lookup_vaddr(vaddr_t vaddr);

	/**
	 * @brief Lookups a TLB entry by physical address.
	 *
	 * @param paddr Target physical address.
	 *
	 * @returns Upon successful completion, a pointer to the TLB entry
	 * that matches the physical address @p paddr is returned. If no
	 * entry that meets this criteria is found, @p NULL is returned.
	 */
	extern const struct tlbe *k1b_tlb_lookup_paddr(paddr_t paddr);

	/**
	 * @brief Flushes the TLB.
	 */
	extern int k1b_tlb_flush(void);

	/**
	 * @brief Initializes the TLB.
	 */
	extern void k1b_tlb_init(void);

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond k1b
 */

	/**
	 * @brief Software-managed TLB.
	 */
	#define TLB_SOFTWARE

	/**
	 * @brief Provided Interface
	 */
	/**@{*/
	#define __tlbe_st             /**< TLB Entry           */
	#define __tlbe_vaddr_get_fn   /**< tlbe_vaddr_get()    */
	#define __tlbe_paddr_get_fn   /**< tlbe_paddr_get()    */
	#define __tlb_lookup_vaddr_fn /**< tlbe_lookup_vaddr() */
	#define __tlb_lookup_paddr_fn /**< tlbe_lookup()       */
	#define __tlb_write_fn        /**< tlb_write()         */
	#define __tlb_inval_fn        /**< tlb_inval()         */
	#define __tlb_flush_fn        /**< tlb_flush()         */
	/**@}*/

	/**
	 * @brief Length of TLB (number of entries).
	 *
	 * Number of entries in the architectural TLB exposed by the
	 * hardware.
	 *
	 * @note The Hypervisor only exposes an interface for playing with
	 * the JTLB, therefore this should not be @p K1B_TLB_SIZE.
	 */
	#define TLB_LENGTH K1B_JTLB_LENGTH

	/**
	 * @name TLB Types
	 */
	/**@{*/
	#define TLB_INSTRUCTION K1B_TLB_INSTRUCTION /**< Instruction TLB */
	#define TLB_DATA        K1B_TLB_DATA        /**< Data TLB        */
	/**@}*/

	/**
	 * @see k1b_tlbe_vaddr_get().
	 */
	static inline vaddr_t tlbe_vaddr_get(const struct tlbe *tlbe)
	{
		return (k1b_tlbe_vaddr_get(tlbe));
	}

	/**
	 * @see k1b_tlbe_paddr_get().
	 */
	static inline paddr_t tlbe_paddr_get(const struct tlbe *tlbe)
	{
		return (k1b_tlbe_paddr_get(tlbe));
	}

	/**
	 * @see k1b_tlb_lookup_vaddr().
	 */
	static inline const struct tlbe *tlb_lookup_vaddr(int tlb_type, vaddr_t vaddr)
	{
		/* Invalid TLB type. */
		if ((tlb_type != K1B_TLB_INSTRUCTION) && (tlb_type != K1B_TLB_DATA))
			return (NULL);

		return (k1b_tlb_lookup_vaddr(vaddr));
	}

	/**
	 * @see k1b_tlb_lookup_paddr().
	 */
	static inline const struct tlbe *tlb_lookup_paddr(int tlb_type, paddr_t paddr)
	{
		/* Invalid TLB type. */
		if ((tlb_type != K1B_TLB_INSTRUCTION) && (tlb_type != K1B_TLB_DATA))
			return (NULL);

		return (k1b_tlb_lookup_paddr(paddr));
	}

	/**
	 * @see k1b_tlb_write()
	 */
	static inline int tlb_write(int tlb_type, vaddr_t vaddr, paddr_t paddr)
	{
		/* Invalid TLB type. */
		if ((tlb_type != K1B_TLB_INSTRUCTION) && (tlb_type != K1B_TLB_DATA))
			return (-EINVAL);

		return (k1b_tlb_write(vaddr, paddr, 12, 0, K1B_TLBE_PROT_RW));
	}

	/**
	 * @see k1b_tlb_inval()
	 */
	static inline int tlb_inval(int tlb_type, vaddr_t vaddr)
	{
		/* Invalid TLB type. */
		if ((tlb_type != K1B_TLB_INSTRUCTION) && (tlb_type != K1B_TLB_DATA))
			return (-EINVAL);

		return (k1b_tlb_inval(vaddr, 12, 0));
	}

	/**
	 * @see k1b_tlb_flush().
	 */
	static inline int tlb_flush(void)
	{
		return (k1b_tlb_flush());
	}

/**@endcond*/

#endif /* ARCH_CORE_K1B_TLB_H_ */
