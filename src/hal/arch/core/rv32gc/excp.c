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

#include <nanvix/const.h>
#include <nanvix/hal/core/exception.h>
#include <nanvix/hlib.h>

/**
 * @cond rv32gc
 *
 * Lookup table with information about exceptions.
 *
 * @endcond
 */
PUBLIC struct exception_info exceptions[RV32GC_EXCP_NUM_EXT] = {
    {NULL, "instruction address misaligned"},
    {NULL, "instruction access fault"},
    {NULL, "illegal instruction"},
    {NULL, "breakpoint"},
    {NULL, "load address misaligned"},
    {NULL, "load access fault"},
    {NULL, "store/amo address misaligned"},
    {NULL, "store/amo access fault"},
    {NULL, "environment call from u-mode"},
    {NULL, "environment call from s-mode"},
    {NULL, "reserved"},
    {NULL, "environment call from m-mode"},
    {NULL, "instruction page fault"},
    {NULL, "load page fault"},
    {NULL, "reserved"},
    {NULL, "page fault"},
    {NULL, "page protection"},
    {NULL, "general protection"},
};

/**
 * @todo TODO provide a detailed description for this function.
 */
PUBLIC void rv32gc_excp_dump(const struct exception *excp)
{
    kprintf("%s", exceptions[excp->num].name);
}
