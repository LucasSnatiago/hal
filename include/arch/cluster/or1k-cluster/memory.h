/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CLUSTER_OR1K_CLUSTER_MEMORY_H_
#define CLUSTER_OR1K_CLUSTER_MEMORY_H_

#ifndef __or1k_cluster__
#error "wrong cluster included!"
#endif

/* Cluster Interface Implementation */
#include <arch/cluster/or1k-cluster/_or1k-cluster.h>

/* Must come first. */
#define __NEED_CLUSTER_MEMMAP

/**
 * @addtogroup or1k-cluster-mem Memory
 * @ingroup or1k-cluster
 *
 * @brief Memory System
 */
/**@{*/

#include <arch/cluster/or1k-cluster/memmap.h>
#include <nanvix/const.h>

/**
 * @name Physical Memory Layout
 */
/**@{*/
#define OR1K_CLUSTER_KERNEL_BASE_PHYS                                          \
    OR1K_CLUSTER_DRAM_BASE_PHYS /**< Kernel Code and Data */
#define OR1K_CLUSTER_KERNEL_END_PHYS                                           \
    (OR1K_CLUSTER_KERNEL_BASE_PHYS + OR1K_PGTAB_SIZE) /**< Kernel End */
#define OR1K_CLUSTER_KPOOL_BASE_PHYS                                           \
    (OR1K_CLUSTER_KERNEL_END_PHYS +                                            \
     OR1K_PGTAB_SIZE) /**< Kernel Page Pool     */
#define OR1K_CLUSTER_KPOOL_END_PHYS                                            \
    (OR1K_CLUSTER_KPOOL_BASE_PHYS + OR1K_PGTAB_SIZE) /**< Kernel Pool End */
#define OR1K_CLUSTER_USER_BASE_PHYS                                            \
    OR1K_CLUSTER_KPOOL_END_PHYS /**< User Base            */
#define OR1K_CLUSTER_USER_END_PHYS                                             \
    OR1K_CLUSTER_DRAM_END_PHYS /**< User End             */
/**@}*/

/**
 * @name Virtual Memory Layout
 */
/**@{*/
#define OR1K_CLUSTER_UART_BASE_VIRT                                            \
    OR1K_CLUSTER_UART_BASE_PHYS /**< UART Base            */
#define OR1K_CLUSTER_UART_END_VIRT                                             \
    OR1K_CLUSTER_UART_END_PHYS /**< UART End             */
#define OR1K_CLUSTER_ETH_BASE_VIRT                                             \
    OR1K_CLUSTER_ETH_BASE_PHYS /**< ETH Base             */
#define OR1K_CLUSTER_ETH_END_VIRT                                              \
    OR1K_CLUSTER_ETH_END_PHYS /**< ETH End              */
#define OR1K_CLUSTER_OMPIC_BASE_VIRT                                           \
    OR1K_CLUSTER_OMPIC_BASE_PHYS /**< PIC Base             */
#define OR1K_CLUSTER_OMPIC_END_VIRT                                            \
    OR1K_CLUSTER_OMPIC_END_PHYS /**< PIC End              */
#define OR1K_CLUSTER_KERNEL_BASE_VIRT                                          \
    OR1K_CLUSTER_KERNEL_BASE_PHYS /**< Kernel Code and Data */
#define OR1K_CLUSTER_KERNEL_END_VIRT                                           \
    OR1K_CLUSTER_KERNEL_END_PHYS /**< Kernel End           */
#define OR1K_CLUSTER_KPOOL_BASE_VIRT                                           \
    OR1K_CLUSTER_KPOOL_BASE_PHYS /**< Kernel Page Pool     */
#define OR1K_CLUSTER_KPOOL_END_VIRT                                            \
    OR1K_CLUSTER_KPOOL_END_PHYS                  /**< Kernel Pool End      */
#define OR1K_CLUSTER_USER_BASE_VIRT 0xa0000000   /**< User Base            */
#define OR1K_CLUSTER_USER_END_VIRT 0xc0000000    /**< User End             */
#define OR1K_CLUSTER_USTACK_BASE_VIRT 0xc0000000 /**< User Stack Base      */
#define OR1K_CLUSTER_USTACK_END_VIRT 0xb0000000  /**< User Stack End       */
/**@}*/

/**
 * @name Memory Regions Constants
 */
/**@{*/
#define OR1K_CLUSTER_MEM_REGIONS 5 /**< Memory Regions number.            */
#define OR1K_CLUSTER_ROOT_PGTAB_NUM                                            \
    OR1K_CLUSTER_MEM_REGIONS /**< Root page table size.             */
#define OR1K_CLUSTER_MREGION_PT_ALIGN_START                                    \
    0 /**< MRegion start page table aligned. */
#define OR1K_CLUSTER_MREGION_PT_ALIGN_END                                      \
    2 /**< MRegion end page table aligned.   */
#define OR1K_CLUSTER_MREGION_PG_ALIGN_START                                    \
    2 /**< MRegion start page aligned.       */
#define OR1K_CLUSTER_MREGION_PG_ALIGN_END                                      \
    OR1K_CLUSTER_MEM_REGIONS /**< MRegion end page aligned.         */
/**@}*/

/**
 * @brief Memory size (in bytes).
 */
#define OR1K_CLUSTER_MEM_SIZE OR1K_CLUSTER_DRAM_SIZE

/**
 * @brief Kernel memory size (in bytes).
 */
#define OR1K_CLUSTER_KMEM_SIZE                                                 \
    (OR1K_CLUSTER_KERNEL_END_PHYS - OR1K_CLUSTER_KERNEL_BASE_PHYS)

/**
 * @brief Kernel page pool size (in bytes).
 *
 * @bug FIXME: https://github.com/nanvix/hal/issues/619
 */
#define OR1K_CLUSTER_KPOOL_SIZE                                                \
    ((OR1K_CLUSTER_KPOOL_END_PHYS - OR1K_CLUSTER_KPOOL_BASE_PHYS) / 32)

/**
 * @brief User memory size (in bytes).
 */
#define OR1K_CLUSTER_UMEM_SIZE                                                 \
    (OR1K_CLUSTER_USER_END_PHYS - OR1K_CLUSTER_USER_BASE_PHYS)

/**
 * @brief Kernel stack size (in bytes).
 */
#define OR1K_CLUSTER_KSTACK_SIZE OR1K_PAGE_SIZE

/**
 * OMPIC Registers and flags.
 */
/**@{*/
#define OR1K_OMPIC_CPUBYTES 8
#define OR1K_OMPIC_CTRL(cpu) ((0x0 + ((cpu)*OR1K_OMPIC_CPUBYTES)))
#define OR1K_OMPIC_STAT(cpu) ((0x4 + ((cpu)*OR1K_OMPIC_CPUBYTES)))
#define OR1K_OMPIC_CTRL_IRQ_ACK (1 << 31)
#define OR1K_OMPIC_CTRL_IRQ_GEN (1 << 30)
#define OR1K_OMPIC_CTRL_DST(cpu) (((cpu)&0x3fff) << 16)
#define OR1K_OMPIC_STAT_IRQ_PENDING (1 << 30)
#define OR1K_OMPIC_DATA(x) ((x)&0xffff)
#define OR1K_OMPIC_STAT_SRC(x) (((x) >> 16) & 0x3fff)
/**@}*/

#ifndef _ASM_FILE_

/**
 * @brief Binary Sections
 */
/**@{*/
EXTERN unsigned char __BOOTSTRAP_START; /**< Bootstrap Start */
EXTERN unsigned char __BOOTSTRAP_END;   /**< Bootstrap End   */
EXTERN unsigned char __TEXT_START;      /**< Text Start      */
EXTERN unsigned char __TEXT_END;        /**< Text End        */
EXTERN unsigned char __DATA_START;      /**< Data Start      */
EXTERN unsigned char __DATA_END;        /**< Data End        */
EXTERN unsigned char __BSS_START;       /**< BSS Start       */
EXTERN unsigned char __BSS_END;         /**< BSS End         */
/**@}*/

/**
 * @brief TLB lookup address mask.
 */
#define OR1K_TLB_VADDR_MASK PAGE_MASK

/**
 * @brief Gets the underlying TLB entries.
 *
 * @param _tlb     The TLB of a core.
 * @param tlb_type Type of the underlying TLB entries that we want get.
 *
 * The or1k_cluster_tlb_get_utlb() function returns the architectural
 * TLB entries of a specific TLB type.
 *
 * @returns Initial position of the specific underlying tlb entries.
 */
EXTERN struct tlbe *or1k_cluster_tlb_get_utlb(int tlb_type);

/**
 * @brief Gets the configuration of a TLB Entry.
 *
 * @param vaddr Target virtual address.
 *
 * @return K1B TLB entry does not need configuration.
 */
EXTERN int or1k_cluster_tlb_get_vaddr_info(vaddr_t vaddr);

/**
 * @brief Flushes the TLB.
 */
EXTERN int or1k_cluster_tlb_flush(void);

/**
 * @brief Initializes the TLB.
 */
EXTERN void or1k_cluster_tlb_init(void);

#endif /* _ASM_FILE_ */

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond or1k_cluster
 */

/**
 * @name Exported Constants
 */
#define MEMORY_SIZE                                                            \
    OR1K_CLUSTER_MEM_SIZE /**< @see OR1K_CLUSTER_MEM_SIZE         */
#define KMEM_SIZE                                                              \
    OR1K_CLUSTER_KMEM_SIZE /**< @see OR1K_CLUSTER_KMEM_SIZE        */
#define UMEM_SIZE                                                              \
    OR1K_CLUSTER_UMEM_SIZE /**< @see OR1K_CLUSTER_UMEM_SIZE        */
#define KSTACK_SIZE                                                            \
    OR1K_CLUSTER_KSTACK_SIZE /**< @see OR1K_CLUSTER_KSTACK_SIZE      */
#define KPOOL_SIZE                                                             \
    OR1K_CLUSTER_KPOOL_SIZE /**< @see OR1K_CLUSTER_KPOOL_SIZE       */
#define KBASE_PHYS                                                             \
    OR1K_CLUSTER_KERNEL_BASE_PHYS /**< @see OR1K_CLUSTER_KERNEL_BASE_PHYS */
#define KPOOL_PHYS                                                             \
    OR1K_CLUSTER_KPOOL_BASE_PHYS /**< @see OR1K_CLUSTER_KPOOL_BASE_PHYS  */
#define UBASE_PHYS                                                             \
    OR1K_CLUSTER_USER_BASE_PHYS /**< @see OR1K_CLUSTER_USER_BASE_PHYS   */
#define USTACK_VIRT                                                            \
    OR1K_CLUSTER_USTACK_BASE_VIRT /**< @see OR1K_CLUSTER_USTACK_BASE_VIRT */
#define UBASE_VIRT                                                             \
    OR1K_CLUSTER_USER_BASE_VIRT /**< @see OR1K_CLUSTER_USER_BASE_VIRT   */
#define UEND_VIRT                                                              \
    OR1K_CLUSTER_USER_END_VIRT /**< @see OR1K_CLUSTER_USER_END_VIRT    */
#define KBASE_VIRT                                                             \
    OR1K_CLUSTER_KERNEL_BASE_VIRT /**< @see OR1K_CLUSTER_KERNEL_BASE_VIRT */
#define KPOOL_VIRT                                                             \
    OR1K_CLUSTER_KPOOL_BASE_VIRT /**< @see OR1K_CLUSTER_KPOOL_BASE_VIRT  */
#define _UART_ADDR                                                             \
    OR1K_CLUSTER_UART_BASE_PHYS /**< @see OR1K_CLUSTER_UART_BASE_PHYS   */
#define TLB_VADDR_MASK                                                         \
    OR1K_TLB_VADDR_MASK /**< @see OR1K_TLB_VADDR_MASK           */
/**@}*/

/**
 * @name Exported Memory Region Constants
 */
/**@{*/
#define MEM_REGIONS                                                            \
    OR1K_CLUSTER_MEM_REGIONS /**< @see OR1K_CLUSTER_MEM_REGIONS            */
#define ROOT_PGTAB_NUM                                                         \
    OR1K_CLUSTER_ROOT_PGTAB_NUM /**< @see OR1K_CLUSTER_ROOT_PGTAB_NUM */
#define MREGION_PT_ALIGN_START                                                 \
    OR1K_CLUSTER_MREGION_PT_ALIGN_START /**< @see                              \
                                           OR1K_CLUSTER_MREGION_PT_ALIGN_START \
                                         */
#define MREGION_PT_ALIGN_END                                                   \
    OR1K_CLUSTER_MREGION_PT_ALIGN_END /**< @see                                \
                                         OR1K_CLUSTER_MREGION_PT_ALIGN_END */
#define MREGION_PG_ALIGN_START                                                 \
    OR1K_CLUSTER_MREGION_PG_ALIGN_START /**< @see                              \
                                           OR1K_CLUSTER_MREGION_PG_ALIGN_START \
                                         */
#define MREGION_PG_ALIGN_END                                                   \
    OR1K_CLUSTER_MREGION_PG_ALIGN_END /**< @see                                \
                                         OR1K_CLUSTER_MREGION_PG_ALIGN_END */
/**@}*/

#ifndef _ASM_FILE_

/**
 * @name Provided Interface
 */
/**@{*/
#define __tlb_flush_fn          /**< tlb_flush()          */
#define __tlbe_dump_fn          /**< tlb_dump()           */
#define __tlb_init_fn           /**< tlb_init()           */
#define __tlb_get_vaddr_info_fn /**< tlb_get_vaddr_info() */
#define __tlb_get_utlb_fn       /**< tlb_get_utlb()       */
/**@}*/

/**
 * @see or1k_cluster_tlb_flush().
 */
static inline int tlb_flush(void)
{
    return (or1k_cluster_tlb_flush());
}

/**
 * @see or1k_cluster_tlb_flush().
 */
static inline int tlbe_dump(int tlb_type, unsigned idx)
{
    UNUSED(tlb_type);
    UNUSED(idx);

    return (0);
}

/**
 * @brief Initializes the TLB in the underlying core.
 */
static inline void tlb_init(void)
{
    or1k_cluster_tlb_init();
}

/**
 * @see or1k_cluster_tlb_get_vaddr_info().
 */
static inline int tlb_get_vaddr_info(vaddr_t vaddr)
{
    return (or1k_cluster_tlb_get_vaddr_info(vaddr));
}

/**
 * @see or1k_cluster_tlb_get_utlb().
 */
static inline struct tlbe *tlb_get_utlb(int tlb_type)
{
    /* Invalid TLB type. */
    if ((tlb_type != OR1K_TLB_INSTRUCTION) && (tlb_type != OR1K_TLB_DATA))
        return (NULL);

    return (or1k_cluster_tlb_get_utlb(tlb_type));
}

#endif /* _ASM_FILE_ */

/**@endcond*/

#endif /* CLUSTER_OR1K_CLUSTER_MEMORY_H_ */
