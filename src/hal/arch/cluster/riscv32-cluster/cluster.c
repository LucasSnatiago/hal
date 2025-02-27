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

#define __NEED_HAL_CLUSTER
#include <nanvix/const.h>
#include <nanvix/hal/cluster.h>
#include <nanvix/hlib.h>

/* Import definitions. */
EXTERN NORETURN void kmain(int, const char *[]);

/**
 * @brief Cores table.
 */
PUBLIC struct coreinfo cores[RISCV32_CLUSTER_NUM_CORES] = {
    {true, CORE_RUNNING, 0, NULL, RV32GC_SPINLOCK_UNLOCKED}, /* Master Core   */
    {false,
     CORE_RESETTING,
     0,
     NULL,
     RV32GC_SPINLOCK_LOCKED}, /* Slave Core 1  */
    {false,
     CORE_RESETTING,
     0,
     NULL,
     RV32GC_SPINLOCK_LOCKED}, /* Slave Core 2  */
    {false,
     CORE_RESETTING,
     0,
     NULL,
     RV32GC_SPINLOCK_LOCKED}, /* Slave Core 3  */
    {false,
     CORE_RESETTING,
     0,
     NULL,
     RV32GC_SPINLOCK_LOCKED}, /* Slave Core 4  */
};

/*============================================================================*
 * riscv32_cluster_setup()                                                    *
 *============================================================================*/

PUBLIC void riscv32_cluster_setup(void)
{
    int coreid;

    coreid = rv32gc_core_get_id();

    if (coreid == RISCV32_CLUSTER_COREID_MASTER)
        kprintf("[hal] booting up cluster...");

    mem_setup();
    core_setup(NULL);
}

/*============================================================================*
 * riscv32_slave_setup()                                                      *
 *============================================================================*/

/**
 * @brief Initializes a slave core.
 *
 * The riscv32_slave_setup() function initializes the underlying slave
 * core.  It setups the stack and then call the kernel main function.
 * Architectural structures are initialized by the master core and
 * registered later on, when the slave core is started effectively.
 *
 * @note This function does not return.
 *
 * @see rv32gc_core_setup() and riscv32_master_setup().
 *
 * @author Pedro Henrique Penna
 */
PUBLIC NORETURN void riscv32_cluster_slave_setup(void)
{
    riscv32_cluster_setup();

    while (true) {
        core_idle();
        core_run();
    }
}

/*============================================================================*
 * riscv32_master_setup()                                                      *
 *============================================================================*/

/**
 * @brief Initializes the master core.
 *
 * The riscv32_master_setup() function initializes the underlying
 * master core. It setups the stack and then call the kernel
 * main function.
 *
 * @note This function does not return.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC NORETURN void riscv32_cluster_master_setup(void)
{
    riscv32_cluster_setup();

    kmain(0, NULL);
}
