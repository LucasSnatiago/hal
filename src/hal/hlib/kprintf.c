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
#include <nanvix/hlib.h>
#include <posix/stdarg.h>

/**
 * @brief Writes a formated string on the kernels's output device.
 *
 * @param fmt Formated string.
 */
PUBLIC void kprintf(const char *fmt, ...)
{
    size_t len;                    /* String length.           */
    va_list args;                  /* Variable arguments list. */
    char buffer[KBUFFER_SIZE + 2]; /* Temporary buffer (+2 for \n\0). */

    /* Convert to raw string. */
    va_start(args, fmt);

    /* Give it 1 extra byte because the size includes '\0'. */
    len = kvsnprintf(buffer, KBUFFER_SIZE + 1, fmt, args);
    /* Substitute the ending with \n\0. */
    buffer[++len] = '\n';
    buffer[++len] = '\0';

    va_end(args);

    kputs(buffer);
}
