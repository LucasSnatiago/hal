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

#ifndef ARCH_CORE_RV32GC_CACHE_H_
#define ARCH_CORE_RV32GC_CACHE_H_

/**
 * @addtogroup rv32gc-core-cache Cache
 * @ingroup rv32gc-core
 *
 * @brief Memory Cache
 */
/**@{*/

#include <nanvix/const.h>

/**
 * @brief L1 Cache line shift.
 */
#define RV32GC_CACHE_LINE_SHIFT 6

/**
 * @brief L1 Cache line size (in bytes).
 */
#define RV32GC_CACHE_LINE_SIZE (1 << RV32GC_CACHE_LINE_SHIFT)

#ifndef _ASM_FILE_

/**
 * @brief Flushes the data cache.
 *
 * @note This function flushes the entire data cache.
 */
static inline void rv32gc_dcache_inval(void)
{
    asm volatile("fence" ::: "memory");
}

/**
 * @brief Flushes the instruction cache.
 *
 * @note This function flushes the entire instruction cache.
 */
static inline void rv32gc_icache_inval(void)
{
    asm volatile("fence.i" ::: "memory");
}

#endif

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond rv32gc
 */

/**
 * @name Provided Interface
 */
/**@{*/
#define __dcache_invalidate_fn /**< dcache_invalidate() */
#define __icache_invalidate_fn /**< icache_invalidate() */
/**@}*/

/**
 * @see RV32GC_CACHE_LINE_SIZE.
 */
#define CACHE_LINE_SIZE RV32GC_CACHE_LINE_SIZE

#ifndef _ASM_FILE_

/**
 * @see rv32gc_dcache_inval().
 */
static inline void dcache_invalidate(void)
{
    rv32gc_dcache_inval();
}

/**
 * @see rv32gc_icache_inval().
 */
static inline void icache_invalidate(void)
{
    rv32gc_icache_inval();
}

#endif

/**@endcond*/

#endif /* ARCH_CORE_RV32GC_CACHE_H_ */
