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

/* Must come first. */
#define __NEED_HAL_CLUSTER
#define __NEED_CLUSTER_MEMMAP

#include <arch/cluster/or1k-cluster/cores.h>
#include <arch/cluster/or1k-cluster/memmap.h>
#include <arch/cluster/or1k-cluster/memory.h>
#include <arch/cluster/or1k-cluster/ompic.h>
#include <nanvix/const.h>
#include <nanvix/hal/cluster/mmio.h>
#include <nanvix/hal/core/interrupt.h>
#include <posix/stdint.h>

/*
 * @brief Sends an Inter-processor Interrupt.
 *
 * @param dstcore Destination core to be sent the message.
 * @param data Data to be sent.
 */
PUBLIC void or1k_cluster_ompic_send_ipi(uint32_t dstcore, uint16_t data)
{
    int coreid;      /* Core ID.       */
    uint32_t *ompic; /* OMPIC address. */

    /* Current core. */
    coreid = or1k_core_get_id();

    /* Get address of uart. */
    ompic = mmio_get(OR1K_CLUSTER_OMPIC_BASE_PHYS);

    /* Send IPI. */
    ompic[OR1K_OMPIC_CTRL(coreid) / OR1K_WORD_SIZE] =
        OR1K_OMPIC_CTRL_IRQ_GEN | OR1K_OMPIC_CTRL_DST(dstcore) |
        OR1K_OMPIC_DATA(data);
}

/*============================================================================*
 * or1k_cluster_event_reset()                                                 *
 *============================================================================*/

/**
 * @brief Complete the interrupt that came from another core.
 */
PUBLIC void or1k_cluster_ompic_ack_ipi(void)
{
    int coreid;      /* Core ID.       */
    uint32_t *ompic; /* OMPIC address. */

    /* Get address of uart. */
    ompic = mmio_get(OR1K_CLUSTER_OMPIC_BASE_PHYS);

    /* Current core. */
    coreid = or1k_core_get_id();

    /* ACK IPI. */
    ompic[OR1K_OMPIC_CTRL(coreid) / OR1K_WORD_SIZE] = OR1K_OMPIC_CTRL_IRQ_ACK;
}

/*
 * @brief Setup the OMPIC.
 */
PUBLIC void or1k_cluster_ompic_init(void)
{
    /* noop. */
}
