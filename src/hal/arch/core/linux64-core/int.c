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

#include <arch/core/linux64/int.h>
#include <nanvix/const.h>
#include <nanvix/hal/core/interrupt.h>
#include <nanvix/hlib.h>
#include <stdlib.h>

/**
 * @brief Current level.
 */
PRIVATE int current_it_level = INTERRUPT_LEVEL_NONE;

/**
 * @brief Generic handler of an interrupt
 */
PRIVATE void linux64_do_interrupt(int intnum)
{
    if (intnum == SIGINT)
        kprintf("<Keybord Interrupt> detected ! (%d)", intnum);
    else
        kprintf("<Alarm Interrupt> detected ! (%d)", intnum);

    exit(0);
}

/**
 * @brief List of the signal considered as interrupts
 */
PRIVATE int linux64_int_signals[] = {SIGALRM, SIGINT, -1};

/**
 * @brief interrupt handlers.
 */
PUBLIC void (*interrupt_handlers[LINUX64_INT_MAX_NUM])(int) = {
    NULL,
    NULL,
    linux64_do_interrupt,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    linux64_do_interrupt,
};

/**
 * @brief Enable all the interrupts.
 */
PUBLIC void linux64_interrupts_enable(void)
{
    for (int i = 0; linux64_int_signals[i] != -1; i++)
        signal(linux64_int_signals[i], do_interrupt);

    current_it_level = INTERRUPT_LEVEL_LOW;
}

/**
 * @brief Disable all the interrupts.
 */
PUBLIC void linux64_interrupts_disable(void)
{
    for (int i = 0; linux64_int_signals[i] != -1; i++)
        signal(linux64_int_signals[i], NULL);

    current_it_level = INTERRUPT_LEVEL_NONE;
}

/**
 * @brief Gets interrupt level.
 */
PUBLIC int linux64_interrupts_get_level(void)
{
    return (current_it_level);
}

/**
 * @brief Change interrupt level.
 */
PUBLIC int linux64_interrupts_set_level(int newlevel)
{
    int oldlevel;

    if (newlevel < INTERRUPT_LEVEL_LOW || newlevel > INTERRUPT_LEVEL_NONE)
        return (-EINVAL);

    oldlevel = current_it_level;

    switch (newlevel) {
    /* INTERRUPT_LEVEL_HIGH */
    case 0: {
        linux64_interrupts_enable();
    } break;

    /**
     * INTERRUPT_LEVEL_HIGH:
     * INTERRUPT_LEVEL_MEDIUM:
     */
    case 1: {

        /* Disable SIGINT interrupt. */
        signal(linux64_int_signals[1], NULL);

        current_it_level = newlevel;
    } break;

    /* INTERRUPT_LEVEL_NONE */
    default: {
        linux64_interrupts_disable();
    } break;
    }

    return (oldlevel);
}

/**
 * @brief Unmask a interrupt.
 *
 * @param The number of the interrupt to mask.
 *
 * @return 0 if succesful, negative value otherwise.
 */
PUBLIC int linux64_interrupt_mask(int intnum)
{
    if (signal(intnum, NULL) == SIG_ERR)
        return (-EINVAL);

    return (0);
}

/**
 * @brief Unmask a interrupt.
 *
 * @param The number of the interrupt to unmask.
 *
 * @return 0 if succesful, negative value otherwise.
 */
PUBLIC int linux64_interrupt_unmask(int intnum)
{
    if (signal(intnum, do_interrupt) == SIG_ERR)
        return (-EINVAL);

    return (0);
}

/**
 * @brief Give the next interrupt called while blocked.
 *
 * @return The number of the next interrupt OR a negative value if there is
 * none.
 */
PUBLIC int linux64_interrupt_next(void)
{
    sigset_t set;
    sigpending(&set);

    for (int i = 0; linux64_int_signals[i] != -1; i++) {
        if (sigismember(&set, linux64_int_signals[i]))
            return (linux64_int_signals[i]);
    }

    return (-EINVAL);
}
