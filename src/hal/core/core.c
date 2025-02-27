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

#ifndef __unix64__

/* Must come first. */
#define __NEED_HAL_CORE

#include <nanvix/const.h>
#include <nanvix/hal/core.h>
#include <nanvix/hlib.h>

/*============================================================================*
 * core_halt()                                                                *
 *============================================================================*/

/**
 * The core_halt() function halt the underlying core.
 * After halting a core, instruction execution cannot be
 * resumed on it.
 *
 * @author João Vicente Souto
 */
PUBLIC NORETURN void core_halt(void)
{
    kprintf("[hal][core] halting...");

    /* Disable all interrupts. */
    interrupts_disable();

    /* Stay here forever. */
    UNREACHABLE();
}

/*============================================================================*
 * core_setup()                                                               *
 *============================================================================*/

/**
 * The core_setup() function initializes all architectural structures
 * of the underlying core. It initializes the Memory Management Unit
 * (MMU), Interrupt Vector Table (IVT) as well as performance
 * monitoring registers.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void core_setup(void *stack)
{
    kprintf("[hal][core] booting up core...");

    core_status_set_mode(CORE_STATUS_MODE_INTERRUPT);
    mmu_setup();
    perf_setup();
    ivt_setup(stack);
}

#else
typedef int make_iso_compilers_happy;
#endif
