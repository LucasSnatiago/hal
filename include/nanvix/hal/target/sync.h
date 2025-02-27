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

#ifndef NANVIX_HAL_TARGET_SYNC_H_
#define NANVIX_HAL_TARGET_SYNC_H_

/* Target Interface Implementation */
#include <nanvix/hal/target/_target.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

#if defined(__INTERFACE_CHECK) || defined(__INTERFACE_CHECK_TARGET_AL) ||      \
    defined(__INTERFACE_CHECK_SYNC)

/* Feature Checking */
#ifndef __TARGET_HAS_SYNC
#error "does this target feature a synchronization interface?"
#endif

/* Has Sync Interface */
#if (__TARGET_HAS_SYNC)

/* Constants */
#ifndef SYNC_CREATE_MAX
#error "SYNC_CREATE_MAX not defined"
#endif
#ifndef SYNC_CREATE_OFFSET
#error "SYNC_CREATE_OFFSET not defined"
#endif
#ifndef SYNC_OPEN_MAX
#error "SYNC_OPEN_MAX not defined"
#endif
#ifndef SYNC_OPEN_OFFSET
#error "SYNC_OPEN_OFFSET not defined"
#endif
#ifndef SYNC_ONE_TO_ALL
#error "SYNC_ONE_TO_ALL not defined"
#endif
#ifndef SYNC_ALL_TO_ONE
#error "SYNC_ALL_TO_ONE not defined"
#endif
#ifndef SYNC_IOCTL_SET_ASYNC_BEHAVIOR
#error "SYNC_IOCTL_SET_ASYNC_BEHAVIOR not defined"
#endif

/* Functions */
#ifndef __sync_setup_fn
#error "sync_setup() not defined?"
#endif
#ifndef __sync_create_fn
#error "sync_create() not defined?"
#endif
#ifndef __sync_open_fn
#error "sync_open() not defined?"
#endif
#ifndef __sync_unlink_fn
#error "sync_unlink() not defined?"
#endif
#ifndef __sync_close_fn
#error "sync_close_() not defined?"
#endif
#ifndef __sync_wait_fn
#error "sync_wait() not defined?"
#endif
#ifndef __sync_signal_fn
#error "sync_signal() not defined?"
#endif
#ifndef __sync_ioctl_fn
#error "sync_ioctl() not defined?"
#endif

#endif

#endif

/* Dummy Constants */
#if (!__TARGET_HAS_SYNC)

#define SYNC_ONE_TO_ALL 0
#define SYNC_ALL_TO_ONE 1
#define SYNC_CREATE_MAX 1
#define SYNC_CREATE_OFFSET 0
#define SYNC_OPEN_MAX 1
#define SYNC_OPEN_OFFSET SYNC_CREATE_MAX
#define SYNC_IOCTL_SET_ASYNC_BEHAVIOR 0

#endif /* !__TARGET_HAS_SYNC */

/*============================================================================*
 * Provided Interface                                                         *
 *============================================================================*/

/**
 * @defgroup kernel-hal-target-sync Syncrhonization service
 * @ingroup kernel-hal-target
 *
 * @brief Target Standard Output HAL Interface
 */
/**@{*/

#include <nanvix/const.h>
#include <nanvix/hlib.h>
#include <posix/errno.h>

/**
 * @brief Allocates and configures the receiving side of the synchronization
 * point.
 *
 * @param nodenums IDs of target NoC nodes.
 * @param nnodes   Number of target NoC nodes.
 * @param type     Type of synchronization point.
 *
 * @return The tag of underlying resource ID.
 */
EXTERN int sync_create(const int *nodenums, int nnodes, int type);

/**
 * @brief Allocates and configures the sending side of the synchronization
 * point.
 *
 * @param nodenums Logic IDs of target NoC nodes.
 * @param nnodes   Number of target NoC nodes.
 * @param type     Type of synchronization point.
 *
 * @return The tag of underlying resource ID.
 */
EXTERN int sync_open(const int *nodenums, int nnodes, int type);
/**
 * @brief Releases and cleans receiver buffer.
 *
 * @param syncid Resource ID.
 *
 * @return Zero if free the resource and non zero otherwise.
 */
EXTERN int sync_unlink(int syncid);

/**
 * @brief Releases the sender resources on a specific DMA channel.
 *
 * @param syncid Resource ID.
 *
 * @return Zero if free the resource and non zero otherwise.
 */
EXTERN int sync_close(int syncid);

/**
 * @brief Wait signal on a specific synchronization point.
 *
 * @param syncid ID of the Target Sync.
 *
 * @return Zero if wait signal correctly and non zero otherwise.
 */
EXTERN int sync_wait(int syncid);

/**
 * @brief Send signal on a specific synchronization point.
 *
 * @param syncid Resource ID.
 * @param nodes  IDs of target NoC nodes.
 * @param nnodes Number of target NoC nodes.
 * @param type   Type of synchronization point.
 *
 * @return Zero if send signal correctly and non zero otherwise.
 */
EXTERN int sync_signal(int syncid);

/**
 * @brief Performs control operations in a sync.
 *
 * @param syncid  Target sync.
 * @param request Request.
 * @param ...     Additional arguments.
 *
 * @param Upon successful completion, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
EXTERN int sync_ioctl(int syncid, unsigned request, ...);

/**
 * @brief Initializes the sync interface.
 */
EXTERN void sync_setup(void);

/**@}*/

#endif /* NANVIX_HAL_TARGET_SYNC_H_ */
