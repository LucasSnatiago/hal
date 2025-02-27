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

#ifndef NANVIX_HAL_HAL_H_
#define NANVIX_HAL_HAL_H_

#if !defined(__NANVIX_HAL) && !defined(__NEED_HAL)
#error "include <nanvix/hal.h> instead"
#endif

#undef __NEED_HAL_TARGET
#define __NEED_HAL_TARGET
#include <nanvix/hal/target.h>

/**
 * @addtogroup kernel-hal HAL
 * @ingroup kernel
 *
 * @brief Hardware Abstraction Layer
 */
/**@{*/

#include <nanvix/hal/log.h>
#include <nanvix/hal/resource.h>
#include <nanvix/hal/section_guard.h>
#include <nanvix/const.h>

/**
 * @brief Initializes the HAL.
 */
EXTERN void hal_init(void);

/**@}*/

#endif /* NANVIX_HAL_HAL_H_ */
