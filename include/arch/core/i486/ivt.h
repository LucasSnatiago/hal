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

#ifndef ARCH_CORE_I486_IVT_H_
#define ARCH_CORE_I486_IVT_H_

/**
 * @addtogroup i486-core-ivt IVT
 * @ingroup i486-core
 *
 * @brief Interrupt Vector Table
 */
/**@{*/

#include <nanvix/const.h>
#include <nanvix/hlib.h>

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond i486
 */

#ifndef _ASM_FILE_

#ifdef __NANVIX_HAL

/**
 * @brief Initializes the interrupt vector table.
 *
 * @param stack Interrupt/Exception stack.
 */
EXTERN void ivt_setup(void *stack);

#endif /* __NANVIX_HAL */

#endif /* _ASM_FILE_ */

/**@endcond*/

#endif /* ARCH_CORE_I486_IVT_H_ */
