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
#define __NEED_HAL_CORE

#include <nanvix/const.h>
#include <nanvix/hal/core.h>
#include <nanvix/hlib.h>

/*===========================================================================*
 * __do_kcall()                                                              *
 *===========================================================================*/

/**
 * @brief Generic exception handler.
 */
PUBLIC int __do_kcall(word_t arg0, word_t arg1, word_t arg2, word_t arg3,
                      word_t arg4, word_t kcall_nr)
{
    int ret;
    int modenum;

    /* Set trap execution mode. */
    modenum = core_status_set_mode(CORE_STATUS_MODE_TRAP);

    /* Call kernel call. */
    ret = do_kcall(arg0, arg1, arg2, arg3, arg4, kcall_nr);

    /* Reset trap execution mode. */
    core_status_set_mode(modenum);

    return (ret);
}
