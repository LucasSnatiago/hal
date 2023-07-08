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

#ifndef ARCH_CORE_LINUX64_PERF_H_
#define ARCH_CORE_LINUX64_PERF_H_

/**
 * @addtogroup linux64-core-perf Perf Monitoring
 * @ingroup linux64-core
 *
 * @brief Performance Monitoring Interface
 */
/**@{*/

/* Must come first. */
#define __NEED_CC

#include <nanvix/cc.h>
#include <linux/perf_event.h>
#include <posix/stdint.h>

/**
 * @brief Number of performance events.
 */
#define LINUX64_PERF_EVENTS_NUM 14

/**
 * @brief Number of performance monitors.
 */
#define LINUX64_PERF_MONITORS_NUM 1

/**
 * @name Performance Events
 */
/**@{*/
#define LINUX64_PERF_CYCLES                                                    \
    PERF_COUNT_HW_CPU_CYCLES /**< Timer Cycles                    */
#define LINUX64_PERF_ICACHE_HITS                                               \
    (PERF_COUNT_HW_CACHE_L1I) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |           \
        (PERF_COUNT_HW_CACHE_RESULT_MISS                                       \
         << 16) /**< Instruction Cache Hits          */
#define LINUX64_PERF_ICACHE_MISSES                                             \
    (PERF_COUNT_HW_CACHE_L1I) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |           \
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS                                     \
         << 16) /**< Instruction Cache Misses        */
#define LINUX64_PERF_ICACHE_STALLS                                             \
    (PERF_COUNT_HW_CACHE_L1I) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |           \
        (PERF_COUNT_HW_CACHE_RESULT_MISS                                       \
         << 16) /**< Instruction Cache Misses Stalls */
#define LINUX64_PERF_DCACHE_HITS                                               \
    (PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |           \
        (PERF_COUNT_HW_CACHE_RESULT_MISS << 16) /**< Data Cache Hits */
#define LINUX64_PERF_DCACHE_MISSES                                             \
    (PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |           \
        (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16) /**< Data Cache Misses */
#define LINUX64_PERF_DCACHE_STALLS                                             \
    (PERF_COUNT_HW_CACHE_L1D) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |           \
        (PERF_COUNT_HW_CACHE_RESULT_MISS                                       \
         << 16)                /**< Data Cache Misses Stalls        */
#define LINUX64_PERF_BUNDLES 0 /**< Bundles Executed                */
#define LINUX64_PERF_BRANCH_TAKEN                                              \
    PERF_COUNT_HW_BRANCH_INSTRUCTIONS /**< Branches Taken                  */
#define LINUX64_PERF_BRANCH_STALLS                                             \
    PERF_COUNT_HW_BRANCH_MISSES /**< Branches Stalled                */
#define LINUX64_PERF_REG_STALLS                                                \
    PERF_SAMPLE_REGS_INTR /**< Register Dependence Stalls      */
#define LINUX64_PERF_ITLB_STALLS                                               \
    (PERF_COUNT_HW_CACHE_ITLB) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |          \
        (PERF_COUNT_HW_CACHE_RESULT_MISS                                       \
         << 16) /**< Instruction TLB Stalls          */
#define LINUX64_PERF_DTLB_STALLS                                               \
    (PERF_COUNT_HW_CACHE_DTLB) | (PERF_COUNT_HW_CACHE_OP_READ << 8) |          \
        (PERF_COUNT_HW_CACHE_RESULT_MISS << 16) /**< Data TLB Stalls */
#define LINUX64_PERF_STREAM_STALLS                                             \
    PERF_SAMPLE_REGS_INTR /**< Stream Buffer Stalls            */
#define LINUX64_PERF_NUM_INSTRUCTIONS                                          \
    PERF_COUNT_HW_INSTRUCTIONS /**< Hardware Instructions            */
/**@}*/

#define LINUX64_PERF_ARG1 0  /**< Default arg1 */
#define LINUX64_PERF_ARG2 -1 /**< Default arg2 */
#define LINUX64_PERF_ARG3 -1 /**< Default arg3 */
#define LINUX64_PERF_ARG4 0  /**< Default arg4 */

/**
 * @Brief Array of File Descriptor containing the values of the counters
 */
extern int linux64_perf_monitors[LINUX64_PERF_MONITORS_NUM];

/**
 * @Brief Check if the perf and event values are valide
 */
extern int perf_isvalid(int perf);

extern int event_isvalid(int event);

/**
 * @brief Initializes performance monitors.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern void linux64_perf_setup(void);

/**
 * @brief Starts a performance monitor.
 *
 * @param perf  Target performance monitor.
 * @param event Target event to watch.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative error code is returned instead.
 */
extern int linux64_perf_start(int perf, int event);

/**
 * @brief Stops a performance monitor.
 *
 * @param perf Target performance monitor.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int linux64_perf_stop(int perf);

/**
 * @brief Restarts a performance monitor.
 *
 * @param perf Target performance monitor.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int linux64_perf_restart(int perf);

/**
 * @brief Reads a PM register.
 *
 * @param perf Target performance monitor.
 *
 * @returns Upon successful completion, the value of the target
 * performance monitor.
 */
extern uint64_t linux64_perf_read(int perf);

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond linux64
 */

/**
 * @name Exported Constants
 */
/**@{*/
#define PERF_MONITORS_NUM                                                      \
    LINUX64_PERF_MONITORS_NUM /**< @ref LINUX64_PERF_MONITORS_NUM     */
#define PERF_EVENTS_NUM                                                        \
    LINUX64_PERF_EVENTS_NUM /**< @ref LINUX64_PERF_EVENTS_NUM       */
#define PERF_CYCLES                                                            \
    LINUX64_PERF_CYCLES /**< @ref LINUX64_PERF_CYCLES           */
#define PERF_ICACHE_HITS                                                       \
    LINUX64_PERF_ICACHE_HITS /**< @ref LINUX64_PERF_ICACHE_HITS      */
#define PERF_ICACHE_MISSES                                                     \
    LINUX64_PERF_ICACHE_MISSES /**< @ref LINUX64_PERF_ICACHE_MISSES    */
#define PERF_ICACHE_STALLS                                                     \
    LINUX64_PERF_ICACHE_STALLS /**< @ref LINUX64_PERF_ICACHE_STALLS    */
#define PERF_DCACHE_HITS                                                       \
    LINUX64_PERF_DCACHE_HITS /**< @ref LINUX64_PERF_DCACHE_HITS      */
#define PERF_DCACHE_MISSES                                                     \
    LINUX64_PERF_DCACHE_MISSES /**< @ref LINUX64_PERF_DCACE_MISSES     */
#define PERF_DCACHE_STALLS                                                     \
    LINUX64_PERF_DCACHE_STALLS /**< @ref LINUX64_PERF_DCACHE_STALLS    */
#define PERF_BUNDLES                                                           \
    LINUX64_PERF_BUNDLES /**< @ref LINUX64_PERF_BUNDLES          */
#define PERF_BRANCH_TAKEN                                                      \
    LINUX64_PERF_BRANCH_TAKEN /**< @ref LINUX64_PERF_BRANCH_TAKEN     */
#define PERF_BRANCH_STALLS                                                     \
    LINUX64_PERF_BRANCH_STALLS /**< @ref LINUX64_PERF_BRANCH_STALLS    */
#define PERF_REG_STALLS                                                        \
    LINUX64_PERF_REG_STALLS /**< @ref LINUX64_PERF_REG_STALLS       */
#define PERF_ITLB_STALLS                                                       \
    LINUX64_PERF_ITLB_STALLS /**< @ref LINUX64_PERF_ITLB_STALLS      */
#define PERF_DTLB_STALLS                                                       \
    LINUX64_PERF_DTLB_STALLS /**< @ref LINUX64_PERF_DTLB_STALLS      */
#define PERF_STREAM_STALLS                                                     \
    LINUX64_PERF_STREAM_STALLS /**< @ref LINUX64_PERF_STREAM_STALLS    */
#define PERF_NUM_INSTRUCTIONS                                                  \
    LINUX64_PERF_NUM_INSTRUCTIONS /**< @ref LINUX64_PERF_NUM_INSTRUCTIONS */

/**@}*/

/**
 * @name Exported Functions
 */
/**@{*/
#define __perf_setup_fn   /**< perf_setup()   */
#define __perf_start_fn   /**< perf_start()   */
#define __perf_stop_fn    /**< perf_stop()    */
#define __perf_restart_fn /**< perf_restart() */
#define __perf_read_fn    /**< perf_read()    */
/**@}*/

/**
 * @see linux64_perf_setup().
 */
static inline void perf_setup(void)
{
    linux64_perf_setup();
}

/**
 * @see linux64_perf_start().
 */
static inline int perf_start(int perf, int event)
{
    return (linux64_perf_start(perf, event));
}

/**
 * @see linux64_perf_stop().
 */
static inline int perf_stop(int perf)
{
    return (linux64_perf_stop(perf));
}

/**
 * @see linux64_perf_restart().
 */
static inline int perf_restart(int perf)
{
    return (linux64_perf_restart(perf));
}

/**
 * @see linux64_perf_read().
 */
static inline uint64_t perf_read(int perf)
{
    return (linux64_perf_read(perf));
}

/**@endcond*/

#endif /* ARCH_CORE_LINUX64_PERF_H_ */
