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

#ifndef ARCH_CORE_RV32GC_CTX_H_
#define ARCH_CORE_RV32GC_CTX_H_

#ifndef __NEED_CORE_CONTEXT
#error "do not include this file"
#endif

/**
 * @addtogroup rv32gc-core-context Context
 * @ingroup rv32gc-core
 *
 * @brief Execution Context Interface
 */
/**@{*/

#include <arch/core/rv32gc/core.h>
#include <nanvix/hlib.h>

/**
 * @brief Execution context size (in bytes).
 */
#define RV32GC_CONTEXT_SIZE 128

/**
 * @name Offsets to the Execution Context structure
 */
/**@{*/
#define RV32GC_CONTEXT_RA                                                      \
    0 * RV32GC_WORD_SIZE /**< Return Address Register      */
#define RV32GC_CONTEXT_SP                                                      \
    1 * RV32GC_WORD_SIZE /**< Stack Pointer Register       */
#define RV32GC_CONTEXT_GP                                                      \
    2 * RV32GC_WORD_SIZE /**< Global Pointer Register      */
#define RV32GC_CONTEXT_TP                                                      \
    3 * RV32GC_WORD_SIZE /**< Thread Pointer Register      */
#define RV32GC_CONTEXT_T0                                                      \
    4 * RV32GC_WORD_SIZE /**< Temporary Register 0         */
#define RV32GC_CONTEXT_T1                                                      \
    5 * RV32GC_WORD_SIZE /**< Temporary Register 1         */
#define RV32GC_CONTEXT_T2                                                      \
    6 * RV32GC_WORD_SIZE /**< Temporary Register 2         */
#define RV32GC_CONTEXT_FP                                                      \
    7 * RV32GC_WORD_SIZE /**< Frame Pointer Register       */
#define RV32GC_CONTEXT_S1                                                      \
    8 * RV32GC_WORD_SIZE /**< Saved Register 1             */
#define RV32GC_CONTEXT_A0                                                      \
    9 * RV32GC_WORD_SIZE /**< Function Argument Register 0 */
#define RV32GC_CONTEXT_A1                                                      \
    10 * RV32GC_WORD_SIZE /**< Function Argument Register 1 */
#define RV32GC_CONTEXT_A2                                                      \
    11 * RV32GC_WORD_SIZE /**< Function Argument Register 2 */
#define RV32GC_CONTEXT_A3                                                      \
    12 * RV32GC_WORD_SIZE /**< Function Argument Register 3 */
#define RV32GC_CONTEXT_A4                                                      \
    13 * RV32GC_WORD_SIZE /**< Function Argument Register 4 */
#define RV32GC_CONTEXT_A5                                                      \
    14 * RV32GC_WORD_SIZE /**< Function Argument Register 5 */
#define RV32GC_CONTEXT_A6                                                      \
    15 * RV32GC_WORD_SIZE /**< Function Argument Register 6 */
#define RV32GC_CONTEXT_A7                                                      \
    16 * RV32GC_WORD_SIZE /**< Function Argument Register 7 */
#define RV32GC_CONTEXT_S2                                                      \
    17 * RV32GC_WORD_SIZE /**< Saved Register 2             */
#define RV32GC_CONTEXT_S3                                                      \
    18 * RV32GC_WORD_SIZE /**< Saved Register 3             */
#define RV32GC_CONTEXT_S4                                                      \
    19 * RV32GC_WORD_SIZE /**< Saved Register 4             */
#define RV32GC_CONTEXT_S5                                                      \
    20 * RV32GC_WORD_SIZE /**< Saved Register 5             */
#define RV32GC_CONTEXT_S6                                                      \
    21 * RV32GC_WORD_SIZE /**< Saved Register 6             */
#define RV32GC_CONTEXT_S7                                                      \
    22 * RV32GC_WORD_SIZE /**< Saved Register 7             */
#define RV32GC_CONTEXT_S8                                                      \
    23 * RV32GC_WORD_SIZE /**< Saved Register 8             */
#define RV32GC_CONTEXT_S9                                                      \
    24 * RV32GC_WORD_SIZE /**< Saved Register 9             */
#define RV32GC_CONTEXT_S10                                                     \
    25 * RV32GC_WORD_SIZE /**< Saved Register 10            */
#define RV32GC_CONTEXT_S11                                                     \
    26 * RV32GC_WORD_SIZE /**< Saved Register 11            */
#define RV32GC_CONTEXT_T3                                                      \
    27 * RV32GC_WORD_SIZE /**< Temporary Register 3         */
#define RV32GC_CONTEXT_T4                                                      \
    28 * RV32GC_WORD_SIZE /**< Temporary Register 4         */
#define RV32GC_CONTEXT_T5                                                      \
    29 * RV32GC_WORD_SIZE /**< Temporary Register 5         */
#define RV32GC_CONTEXT_T6                                                      \
    30 * RV32GC_WORD_SIZE /**< Temporary Register 6         */
#define RV32GC_CONTEXT_PC                                                      \
    31 * RV32GC_WORD_SIZE /**< Program Counter              */
                          /**@}*/

#ifndef _ASM_FILE_

/**
 * @cond rv32gc
 */

/**
 * Saved execution context upon interrupts and exceptions.
 */
struct context {
    rv32gc_word_t ra;         /**< Return Address Register                  */
    rv32gc_word_t sp;         /**< Stack Pointer Register                   */
    rv32gc_word_t gp;         /**< Global Pointer Register                  */
    rv32gc_word_t tp;         /**< Thread Pointer Register                  */
    rv32gc_word_t t0, t1, t2; /**< Temporary Registers 0 to 2               */
    rv32gc_word_t fp;         /**< Frame Pointer Register                   */
    rv32gc_word_t s1;         /**< Saved Register 1                         */
    rv32gc_word_t a0, a1;     /**< Function Argument/Return Value Registers */
    rv32gc_word_t a2, a3, a4, a5, a6,
        a7; /**< Function Argument Registers 2 to 7       */
    rv32gc_word_t s2, s3, s4, s5, s6;   /**< Saved Registers 2 to 6   */
    rv32gc_word_t s7, s8, s9, s10, s11; /**< Saved Registers 7 to 11 */
    rv32gc_word_t t3, t4, t5, t6;       /**< Temporary Registers 3 to 6       */
    rv32gc_word_t pc; /**< Program Counter                          */
} PACK ALIGN(RV32GC_WORD_SIZE);

/**@endcond*/

/**
 * @brief Dumps context information.
 *
 * @param ctx Saved execution context.
 */
EXTERN void rv32gc_context_dump(const struct context *ctx);

/**
 * @brief Gets the value of the stack pointer register.
 *
 * @param ctx Target context.
 *
 * @returns The value of the stack pointer register, which is
 * saved in the saved execution context pointed to by @p ctx.
 */
static inline rv32gc_word_t rv32gc_context_get_sp(const struct context *ctx)
{
    return (ctx->sp);
}

/**
 * @brief Gets the value of the program conter register.
 *
 * @param ctx Target context.
 *
 * @returns The value of the program conter register, which is
 * saved in the saved execution context pointed to by @p ctx.
 */
static inline rv32gc_word_t rv32gc_context_get_pc(const struct context *ctx)
{
    return (ctx->pc);
}

/**
 * @brief Sets the value of the stack pointer register.
 *
 * @param ctx Target context.
 * @param val Value to store.
 */
static inline void rv32gc_context_set_sp(struct context *ctx, rv32gc_word_t val)
{
    ctx->sp = val;
}

/**
 * @brief Sets the value of the program conter register.
 *
 * @param ctx Target context.
 * @param val Value to store.
 */
static inline void rv32gc_context_set_pc(struct context *ctx, rv32gc_word_t val)
{
    ctx->pc = val;
}

#endif /* _ASM_FILE_ */

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond rv32gc
 */

/*
 * Exported Constants
 */
/**@{*/
#define CONTEXT_SIZE RV32GC_CONTEXT_SIZE /**< @see RV32GC_CONTEXT_SIZE */
/**@}*/

/**
 * @name Exported Structures
 */
/**@{*/
#define __context_struct /**< @see context */
/**@}*/

/**
 * @brief Exported Functions
 */
/**@{*/
#define __context_get_sp_fn /**< context_get_sp() */
#define __context_get_pc_fn /**< context_get_pc() */
#define __context_set_sp_fn /**< context_set_sp() */
#define __context_set_pc_fn /**< context_set_pc() */
#define __context_dump_fn   /**< context_dump()   */
                            /**@}*/

#ifndef _ASM_FILE_

/**
 * @see rv32gc_context_get_sp().
 */
static inline rv32gc_word_t context_get_sp(const struct context *ctx)
{
    return (rv32gc_context_get_sp(ctx));
}

/**
 * @see rv32gc_context_get_pc().
 */
static inline rv32gc_word_t context_get_pc(const struct context *ctx)
{
    return (rv32gc_context_get_pc(ctx));
}

/**
 * @see rv32gc_context_set_sp().
 */
static inline void context_set_sp(struct context *ctx, rv32gc_word_t val)
{
    rv32gc_context_set_sp(ctx, val);
}

/**
 * @see rv32gc_context_set_pc().
 */
static inline void context_set_pc(struct context *ctx, rv32gc_word_t val)
{
    rv32gc_context_set_pc(ctx, val);
}

/**
 * @see rv32gc_context_dump().
 */
static inline void context_dump(const struct context *ctx)
{
    rv32gc_context_dump(ctx);
}

#endif /* _ASM_FILE_ */

/**@endcond*/

#endif /* ARCH_CORE_RV32GC_CTX_H_ */
