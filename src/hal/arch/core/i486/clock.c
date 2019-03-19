/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
#include <arch/core/i486/8253.h>
#include <arch/core/i486/pmio.h>

/**
 * The i486_clock_init() function initializes the clock driver in the
 * i486 architecture. The frequency of the device is set to @p freq
 * Hz.
 */
PUBLIC void i486_clock_init(unsigned freq)
{
	uint16_t freq_divisor;
	
	freq_divisor = PIT_FREQUENCY/freq;
	
	/* Send control byte: adjust frequency divisor. */
	i486_output8(PIT_CTRL, 0x36);
	
	/* Send data byte: divisor_low and divisor_high. */
	i486_output8(PIT_DATA, (uint8_t)(freq_divisor & 0xff));
	i486_output8(PIT_DATA, (uint8_t)((freq_divisor >> 8)));
}

