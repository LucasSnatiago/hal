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

#include <nanvix/hal/core/interrupt.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <errno.h>

/**
 * @brief Hardware interrupts table.
 */
PRIVATE struct
{
	int handled; /**< Handled? */
} interrupts[INTERRUPTS_NUM];

/**
 * @brief Clock handler.
 */
PRIVATE interrupt_handler_t clock_handler = NULL;

/**
 * @brief Number of spurious interrupts.
 */
PRIVATE unsigned spurious = 0;

/**
 * @brief Default hardware interrupt handler.
 *
 * @param num Number of triggered interrupt.
 */
PRIVATE void default_handler(int num)
{
	UNUSED(num);

	/* Too many spurious interrupts. */
	if (++spurious >= INTERRUPT_SPURIOUS_THRESHOLD)
		kprintf("[hal] spurious interrupt %d", num);

	noop();
}

/**
 * @brief Wrapper for clock interrupt.
 */
PRIVATE void do_clock(int num)
{
	/* Forward clock interrupt handling. */
	if (clock_handler != NULL)
		clock_handler(num);

	clock_reset();
}

/**
 * The interrupt_register() function registers @p handler as the
 * handler function for the interrupt whose number is @p num. If a
 * handler function was previously registered with this number, the
 * interrupt_register() function fails.
 */
PUBLIC int interrupt_register(int num, interrupt_handler_t handler)
{
	/* Invalid interrupt number. */
	if ((num < 0) || (num >= INTERRUPTS_NUM))
		return (-EINVAL);

	/* Handler function already registered. */
	if (interrupts[num].handled)
		return (-EBUSY);

	interrupts[num].handled = TRUE;
	dcache_invalidate();

	if (num != INTERRUPT_CLOCK)
		interrupt_set_handler(num, handler);
	else
	{
		clock_handler = handler;
		interrupt_set_handler(num, do_clock);
	}

	interrupt_unmask(num);

	kprintf("[hal] interrupt handler registered for irq %d", num);

	return (0);
}

/**
 * The interrupt_unregister() function unregisters a handler function
 * for the interrupt whose number is @p num. If no handler function
 * was previously registered with this number, the
 * interrupt_unregister() function fails.
 */
PUBLIC int interrupt_unregister(int num)
{
	/* Invalid interrupt number. */
	if ((num < 0) || (num >= INTERRUPTS_NUM))
		return (-EINVAL);

	/* No handler function is registered. */
	if (!interrupts[num].handled)
		return (-EINVAL);

	interrupts[num].handled = FALSE;
	dcache_invalidate();

	if (num != INTERRUPT_CLOCK)
		interrupt_set_handler(num, default_handler);
	else
	{
		clock_handler = default_handler;
		interrupt_set_handler(num, do_clock);
	}

	interrupt_mask(num);

	kprintf("[hal] interrupt handler unregistered for irq %d", num);

	return (0);
}

/**
 * Initializes hardware interrupts by registering a default handler to
 * all available interrupts.
 */
PUBLIC void interrupt_setup(void)
{
	kputs("[hal] initializing interrupts...\n");

	for (int i = 0; i < INTERRUPTS_NUM; i++)
	{
		interrupt_handler_t handler;

		handler = (i == INTERRUPT_CLOCK) ? do_clock : default_handler;

		interrupts[i].handled = FALSE;
		dcache_invalidate();

		interrupt_set_handler(i, handler);
	}

	clock_handler = default_handler;
}

