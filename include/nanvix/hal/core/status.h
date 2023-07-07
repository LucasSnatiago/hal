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

#ifndef NANVIX_HAL_CORE_STATUS_H_
#define NANVIX_HAL_CORE_STATUS_H_

/* Core Interface Implementation */
#include <nanvix/hal/core/_core.h>

/*============================================================================*
 * Status Interface                                                           *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-core-status Status
 * @ingroup kernel-hal-core
 *
 * @brief Status
 */
/**@{*/

/**
 * @name Core status modes.
 */
/**@{*/
#define CORE_STATUS_MODE_NORMAL 0    /**< Core is on normal execution. */
#define CORE_STATUS_MODE_INTERRUPT 1 /**< Core is in an interrupt.     */
#define CORE_STATUS_MODE_EXCEPTION 2 /**< Core is in an exception.     */
#define CORE_STATUS_MODE_TRAP 3      /**< Core is in a trap.           */
#define CORE_STATUS_MODE_MASKED 4    /**< Core interrupts were masked. */
#define CORE_STATUS_MODE_LIMIT                                                 \
    (CORE_STATUS_MODE_MASKED + 1) /**< Limiting number.             */
/**@}*/

/**
 * @brief Core Status.
 *
 * @details This structure holds general status informations about the
 * underlying core that is updated by kernels code. In this sense, we can
 * easy consult the execution status of the core in an
 * architecture-independent way.
 */
struct core_status {
    int mode; /**< @see Core status modes. */
} ALIGN(CACHE_LINE_SIZE);

/**
 * @brief Set execution mode.
 */
EXTERN int core_status_set_mode(int mode);

/**
 * @brief Get execution mode.
 */
EXTERN int core_status_get_mode(void);

/**@}*/

#endif /* NANVIX_HAL_CORE_STATUS_H_ */
