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

/* Must come first. */
#define __NEED_HAL_CLUSTER

#include <nanvix/const.h>
#include <nanvix/hal/cluster.h>
#include <nanvix/hlib.h>

/**
 * @brief Status per core.
 *
 * @see include/hal/core/status.h
 */
PRIVATE struct core_status cores_status[CORES_NUM];

/*============================================================================*
 * core_status_set_mode()                                                     *
 *============================================================================*/

/**
 * @brief Set execution mode.
 */
PUBLIC int core_status_set_mode(int mode)
{
    int coreid;
    int oldmode;

    /* Valid mode. */
    KASSERT(WITHIN(mode, CORE_STATUS_MODE_NORMAL, CORE_STATUS_MODE_LIMIT));

    coreid = core_get_id();

    oldmode = cores_status[coreid].mode;
    cores_status[coreid].mode = mode;

    return (oldmode);
}

/*============================================================================*
 * core_status_get_mode()                                                     *
 *============================================================================*/

/**
 * @brief Get execution mode.
 */
PUBLIC int core_status_get_mode(void)
{
    return (cores_status[core_get_id()].mode);
}
