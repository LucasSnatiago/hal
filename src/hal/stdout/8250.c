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

#include <arch/stdout/8250.h>
#include <nanvix/const.h>
#include <nanvix/hal/cluster/mmio.h>

/**
 * @brief Flag that indicates if the device was initialized.
 */
PRIVATE bool initialized = false;

/**
 * Writes into serial port.
 */
PUBLIC void uart8250_write(const char *buf, size_t n)
{
    uint8_t *uart8250;
    size_t counter;
    counter = 0;

    /**
     * It's important to only try to write if the device
     * was already initialized.
     */
    if (!initialized)
        return;

    /* Get address of uart. */
    uart8250 = mmio_get(UART_ADDR);

    while (n) {
        /* Wait until FIFO is empty. */
        while (!(uart8250[LSR] & LSR_TFE))
            ;

        /* Write character to device. */
        uart8250[THR] = buf[counter];

        n--;
        counter++;
    }
}

/**
 * Initializes the serial device.
 */
PUBLIC void uart8250_init(void)
{
    uint8_t *uart8250;
    uint16_t divisor;

    /* Do not re-initialize the device. */
    if (initialized)
        return;

    /* Get address of uart. */
    uart8250 = mmio_get(UART_ADDR);

    /* Calculate and set divisor. */
    divisor = UART_TIMER_SIGNAL / (UART_BAUD << 4);
    uart8250[LCR] = LCR_DLA;
    uart8250[DLB1] = divisor & 0xff;
    uart8250[DLB2] = divisor >> 8;

    /*
     * Set line control register:
     *  - 8 bits per character
     *  - 1 stop bit
     *  - No parity
     *  - Break disabled
     *  - Disallow access to divisor latch
     */
    uart8250[LCR] = LCR_BPC_8;

    /* Reset FIFOs and set trigger level to 1 byte. */
    uart8250[FCR] = FCR_CLRRECV | FCR_CLRTMIT | FCR_TRIG_1;

    /* Disable 'Data Available Interrupt'. */
    uart8250[IER] = 0;

    /* Device initialized. */
    initialized = true;
}
