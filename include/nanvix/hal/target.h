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

#ifndef NANVIX_HAL_TARGET_H_
#define NANVIX_HAL_TARGET_H_

#ifndef __NEED_HAL_TARGET
#error "include <nanvix/hal.h> instead"
#endif

/* Target Interface Implementation */
#include <nanvix/hal/target/_target.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

#if defined(__INTERFACE_CHECK) || defined(__INTERFACE_CHECK_TARGET_AL)

/* Constants */
#ifndef TARGET_FREQUENCY
#error "TARGET_FREQUENCY not defined"
#endif

#endif

/*============================================================================*
 * Target Interface                                                           *
 *============================================================================*/

/**
 * @defgroup kernel-hal-target Target
 * @ingroup kernel-hal
 *
 * @brief Target HAL Interface
 */
/**@{*/

#include <nanvix/hal/target/stdout.h>
#include <nanvix/hal/target/sync.h>
#include <nanvix/hal/target/mailbox.h>
#include <nanvix/hal/target/portal.h>

/**
 * @name Functions to wait/wakeup for a comm resource.
 */
/**{**/
typedef void (*target_comm_wait_fn)(int id);
typedef void (*target_comm_wakeup_fn)(int id);
/**}**/

/**
 * @brief Powers off the underlying target.
 */
#ifdef __target_poweroff_fn
EXTERN NORETURN void target_poweroff(void);
#else
static inline NORETURN void target_poweroff(void)
{
    processor_poweroff();
}
#endif

/**@}*/

#endif /* NANVIX_HAL_TARGET_H_ */
