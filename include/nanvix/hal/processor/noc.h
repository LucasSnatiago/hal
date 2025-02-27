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

#ifndef NANVIX_HAL_PROCESSOR_NOC_H_
#define NANVIX_HAL_PROCESSOR_NOC_H_

/* Processor Interface Implementation */
#include <nanvix/hal/processor/_processor.h>

/* Feature Checking */
#ifndef PROCESSOR_HAS_NOC
#error "Does this processor have Network-on-Chip (NoC) ?"
#endif

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

#if defined(__INTERFACE_CHECK) || defined(__INTERFACE_CHECK_PROCESSOR_AL) ||   \
    defined(__INTERFACE_CHECK_NOC)

/* Feature Checking */
#ifndef PROCESSOR_HAS_NOC
#error "Does this processor have a NoC?"
#endif

#if (PROCESSOR_HAS_NOC)

/* Constants */
#ifndef PROCESSOR_NOC_IONODES_NUM
#error "PROCESSOR_NOC_IONODES not defined"
#endif
#ifndef PROCESSOR_NOC_CNODES_NUM
#error "PROCESSOR_NOC_CNODES not defined"
#endif
#ifndef PROCESSOR_NODENUM_MASTER
#error "PROCESSOR_NODENUM_MASTER not defined"
#endif
#ifndef PROCESSOR_NODENUM_LEADER
#error "PROCESSOR_NODENUM_LEADER not defined"
#endif

/* Functions */
#ifndef __processor_noc_setup_fn
#error "__processor_noc_setup() not defined?"
#endif
#ifndef __processor_noc_is_ionode_fn
#error "__processor_noc_is_ionode() not defined?"
#endif
#ifndef __processor_noc_is_cnode_fn
#error "__processor_noc_is_cnode() not defined?"
#endif
#ifndef __processor_node_get_num_fn
#error "__processor_node_get_num() not defined?"
#endif

#endif

#endif

/*============================================================================*
 * NoC Interface                                                              *
 *============================================================================*/

#if (PROCESSOR_HAS_NOC)

/**
 * @defgroup kernel-hal-processor-noc Network-on-Chip
 * @ingroup kernel-hal-processor
 *
 * @brief Processor NoC HAL Interface
 */
/**@{*/

/**
 * @name Total number of NoC nodes.
 */
#define PROCESSOR_NOC_NODES_NUM                                                \
    (PROCESSOR_NOC_IONODES_NUM + PROCESSOR_NOC_CNODES_NUM)

#ifdef __NANVIX_HAL

/**
 * @brief Initializes the mailbox interface.
 */
EXTERN void processor_noc_setup(void);

#endif /* __NANVIX_HAL */

/**
 * @brief Asserts whether a NoC node is attached to an IO cluster.
 *
 * @param nodenum Logic ID of the target NoC node.
 *
 * @returns One if the target NoC node is attached to an IO cluster,
 * and zero otherwise.
 */
EXTERN int processor_noc_is_ionode(int nodenum);

/**
 * @brief Asserts whether a NoC node is attached to a compute cluster.
 *
 * @param nodenum Logic ID of the target NoC node.
 *
 * @returns One if the target NoC node is attached to a compute
 * cluster, and zero otherwise.
 */
EXTERN int processor_noc_is_cnode(int nodenum);

/**
 * @brief Gets the logic number of the target NoC node
 * attached with a core.
 *
 * @returns The logic number of the target NoC node attached
 * with the @p coreid.
 */
EXTERN int processor_node_get_num(void);

/**
 * @brief Asserts whether or not a node number is valid.
 *
 * @param nodenum ID of the target node number.
 *
 * @returns One if the target node number is valid, and zero otherwise.
 *
 * @note This function is non-blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
static inline int node_is_valid(int nodenum)
{
    return (WITHIN(nodenum, 0, PROCESSOR_NOC_NODES_NUM));
}

/**
 * @brief Assess if the nodenum in the local cluster.
 *
 * @param nodenum Logic ID of a NoC node.
 *
 * @return No zero if nodenum is on local cluster and zero otherwise.
 */
static inline int node_is_local(int nodenum)
{
    return (nodenum == processor_node_get_num());
}

/**@}*/

#endif /* PROCESSOR_HAS_NOC */

#endif /* NANVIX_HAL_PROCESSOR_NOC_H_ */
