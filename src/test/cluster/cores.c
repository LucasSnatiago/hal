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

#include "../test.h"
#include <nanvix/const.h>
#include <nanvix/hal/hal.h>
#include <nanvix/hlib.h>

/**
 * @brief Launch verbose tests?
 */
#define TEST_CORES_VERBOSE 0

/**
 * @brief Number of iterations for stress tests.
 */
#define NITERATIONS 10

/*============================================================================*
 * Dummy Task                                                                 *
 *============================================================================*/

/**
 * @brief Dummy task.
 */
PRIVATE void task(void)
{
    const int n = 1000;
    volatile int k = 0;

    /* Waste some cycles. */
    for (int i = 0; i < n; i++)
        k++;

    KASSERT(k == n);
}

/*============================================================================*
 * Slaves                                                                     *
 *============================================================================*/

/**
 * @brief Slave fence.
 */
PRIVATE struct fence slave_fence;

/*----------------------------------------------------------------------------*
 * Slave                                                                      *
 *----------------------------------------------------------------------------*/

/**
 * @brief Slave.
 */
PRIVATE void slave(void)
{
#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d running", core_get_id());
#endif

    task();

#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d stopping", core_get_id());
#endif

    fence_join(&slave_fence);

    KASSERT(core_release() == 0);
    core_reset();
}

/*----------------------------------------------------------------------------*
 * Slave Reset                                                                *
 *----------------------------------------------------------------------------*/

/**
 * @brief Number of starts of slave core.
 */
PRIVATE volatile int slave_nstarts = 0;

/**
 * @brief Reset slave.
 */
PRIVATE void slave_reset(void)
{
#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d sleeping", core_get_id());
#endif

    slave_nstarts++;
    dcache_invalidate();

#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d resetting", core_get_id());
#endif

    /* If first invocation, lets reset. */
    if (slave_nstarts == 1) {
        KASSERT(core_release() == 0);
        core_reset();
    }

#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d stopping", core_get_id());
#endif
}

/*----------------------------------------------------------------------------*
 * Slave Sleep/Wakeup                                                         *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Core suspend and wakeup flags.
 */
#define TEST_CORE_RUNNING                                                      \
    0xDEAD                      /**< Indicates that the core was suspended.    \
                                 */
#define TEST_CORE_AWAKEN 0xC0DE /**< Indicates that the core is awaken.     */

/**
 * @brief State of slave core.
 */
PRIVATE volatile int slave_state = 0;

/**
 * @brief Sleep/Wakeup slave.
 */
PRIVATE void slave_sleep_wakeup(void)
{
#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d running", core_get_id());
#endif

    /* Running. */
    slave_state = TEST_CORE_RUNNING;
    dcache_invalidate();

#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d sleeping", core_get_id());
#endif

    /* Sleep. */
    core_sleep();

#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d awaken", core_get_id());
#endif

    /* Wakeup. */
    slave_state = TEST_CORE_AWAKEN;
    dcache_invalidate();

#if (TEST_CORES_VERBOSE)
    kprintf("[test][cluster][cores] core %d stopping", core_get_id());
#endif
}

/*============================================================================*
 * Leader                                                                     *
 *============================================================================*/

/**
 * @brief Join fence for leader.
 */
PRIVATE struct fence leader_fence;

/**
 * @brief Number of slaves to start.
 */
PRIVATE int nslaves = 0;

/*----------------------------------------------------------------------------*
 * Leader                                                                     *
 *----------------------------------------------------------------------------*/

/**
 * @brief Leader.
 */
PRIVATE void leader(void)
{
    int _nslaves;
    int mycoreid;

    _nslaves = 0;
    mycoreid = core_get_id();
    fence_init(&slave_fence, nslaves);

    /* Start execution in all cores. */
    for (int i = 0; i < CORES_NUM; i++) {
        if ((i != COREID_MASTER) && (i != mycoreid)) {
            int ret;

            do {
                ret = core_start(i, slave);
                KASSERT((ret == 0) || (ret == -EBUSY));
            } while (ret != 0);

            if (++_nslaves == nslaves)
                break;
        }
    }

    /* Wait for slave cores. */
    fence_wait(&slave_fence);

    fence_join(&leader_fence);

    KASSERT(core_release() == 0);
    core_reset();
}

/*----------------------------------------------------------------------------*
 * Leader Sleep/Wakeup                                                        *
 *----------------------------------------------------------------------------*/

/**
 * @brief Leader Sleep/Wakeup.
 */
PRIVATE void leader_sleep_wakeup(void)
{
    int coreid;
    int mycoreid;

    mycoreid = core_get_id();

    /* Start execution in all cores. */
    for (int i = 0; i < CORES_NUM; i++) {
        if ((i != COREID_MASTER) && (i != mycoreid)) {
            int ret;

            do {
                ret = core_start(coreid = i, slave_sleep_wakeup);
                KASSERT((ret == 0) || (ret == -EBUSY));
            } while (ret != 0);

            break;
        }
    }

    /*
     * Send a wakeup signal to the slave core.
     *
     * Note: It's important to note that the wakeup signal is
     * not atomic, i.e, the signal can arrives before the core
     * slept. The HAL covers this scenario using a wakeups
     * counter that is able to prevent a core from sleeping if
     * it has already received a wakeup signal.
     */
    do
        dcache_invalidate();
    while (slave_state != TEST_CORE_RUNNING);
    KASSERT(core_wakeup(coreid) == 0);

    /* Wait for the slave to wake up. */
    do
        dcache_invalidate();
    while (slave_state != TEST_CORE_AWAKEN);

    fence_join(&leader_fence);
}

/*============================================================================*
 * API Tests                                                                  *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Start Execution Slave                                                      *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Start Execution in a Slave Core
 */
PRIVATE void test_cluster_core_api_start_slave(void)
{
    fence_init(&slave_fence, 1);

    /* Start a slave core. */
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            KASSERT(core_start(i, slave) == 0);
            break;
        }
    }

    fence_wait(&slave_fence);
}

/*----------------------------------------------------------------------------*
 * Stop Execution in a Slave Core                                             *
 *----------------------------------------------------------------------------*/

#ifndef __unix64__

/**
 * @brief API Test: Stop Execution in a Slave Core
 */
PRIVATE void test_cluster_core_api_reset_slave(void)
{
    int ret;
    int coreid;

    coreid = 0;

    /* Reset flag. */
    slave_nstarts = 0;
    dcache_invalidate();

    /* Start a slave core. */
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            core_start(coreid = i, slave_reset);
            break;
        }
    }

    /*
     * Wait indefinitely for the slave start. Note that, if for some
     * reason the slave core not start, the master core will hang
     * forever.
     */
    do
        dcache_invalidate();
    while (slave_nstarts != 1);

    /*
     * Wait for the slave become available again, which should
     * occur when core_start returns 0, meaning that a successful
     * start command was sent to the slave core.
     */
    do {
        ret = core_start(coreid, slave_reset);
        KASSERT((ret == 0) || (ret == -EBUSY));
    } while (ret != 0);

    /*
     * Wait indefinitely for the slave start again, which should
     * happens when the slave_nstarts is incremented again. Note that,
     * if for some reason the slave core not start, the master core
     * will hang forever.
     */
    do
        dcache_invalidate();
    while (slave_nstarts != 2);
}

#endif /* !__unix64__ */

/*----------------------------------------------------------------------------*
 * Suspend and Resume from Master                                             *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Suspend and Resume a Slave Core
 */
PRIVATE void test_cluster_core_api_sleep_wakeup_slave(void)
{
    int coreid;

    coreid = 0;

    /* Start one slave core. */
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            KASSERT(core_start(coreid = i, slave_sleep_wakeup) == 0);
            break;
        }
    }

    /*
     * Send a wakeup signal to the slave core.
     *
     * Note: It's important to note that the wakeup signal is
     * not atomic, i.e, the signal can arrives before the core
     * slept. The HAL covers this scenario using a wakeups
     * counter that is able to prevent a core from sleeping if
     * it has already received a wakeup signal.
     */
    do
        dcache_invalidate();
    while (slave_state != TEST_CORE_RUNNING);
    KASSERT(core_wakeup(coreid) == 0);

    /* Wait for the slave to wake up. */
    do
        dcache_invalidate();
    while (slave_state != TEST_CORE_AWAKEN);
}

/*----------------------------------------------------------------------------*
 * Start Execution in a Slave Core (from Slave Core)                          *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Start Execution in a Slave Core (from Slave Core)
 */
PRIVATE void test_cluster_core_api_start_leader(void)
{
    int coreid;

    /* Test not applicable. */
    if (CORES_NUM <= 2)
        return;

    nslaves = 1;
    dcache_invalidate();

    fence_init(&leader_fence, 1);

    /* Start the first available slave core. */
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            core_start(coreid = i, leader);
            break;
        }
    }

    /*
     * Wait indefinitely for the slave start.
     *
     * @note: If for some reason the slave core not start,
     * the master core will hang forever.
     */
    fence_wait(&leader_fence);
}

/*----------------------------------------------------------------------------*
 * Suspend and Resume Execution in a Slave Core (from Slave Core)             *
 *----------------------------------------------------------------------------*/

/**
 * @brief API Test: Suspend and Resume Execution in a Slave Core (from Slave
 * Core)
 */
PRIVATE void test_cluster_core_api_sleep_wakeup_leader(void)
{
    int coreid;

    /* Test not applicable. */
    if (CORES_NUM <= 2)
        return;

    fence_init(&leader_fence, 1);

    /* Start the first available slave core. */
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            core_start(coreid = i, leader);
            break;
        }
    }

    /*
     * Wait indefinitely for the slave start.
     *
     * @note: If for some reason the slave core not start,
     * the master core will hang forever.
     */
    fence_wait(&leader_fence);
}

/*============================================================================*
 * Fault Injection Tests                                                      *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Start Instruction Execution in the Master Core                             *
 *----------------------------------------------------------------------------*/

/**
 * @brief Fault Injection Tests: Start the Master Core
 */
PRIVATE void test_cluster_core_fault_start_master(void)
{
    KASSERT(core_start(COREID_MASTER, slave) == -EINVAL);
}

/*----------------------------------------------------------------------------*
 * Start a Bad Execution Flow                                                 *
 *----------------------------------------------------------------------------*/

/**
 * @brief Fault Injection Test: Start a Bad Execution Flow
 */
PRIVATE void test_cluster_core_fault_start_bad(void)
{
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            KASSERT(core_start(i, NULL) == -EINVAL);
            break;
        }
    }
}

/*----------------------------------------------------------------------------*
 * Start a Invalid Execution Flow                                             *
 *----------------------------------------------------------------------------*/

/**
 * @brief Fault Injection Tests: Start an Invalid Execution Flow
 */
PRIVATE void test_cluster_core_fault_start_inval(void)
{
    KASSERT(core_start(-1, slave) == -EINVAL);
    KASSERT(core_start(CORES_NUM, slave) == -EINVAL);
}

/*----------------------------------------------------------------------------*
 * Stop Instruction Execution in the Master Core                              *
 *----------------------------------------------------------------------------*/

/**
 * @brief Fault Injection Tests: Stop Execution in the Master Core
 */
PRIVATE void test_cluster_core_fault_reset_master(void)
{
    KASSERT(core_release() == -EINVAL);
    KASSERT(core_reset() == -EINVAL);
}

/*----------------------------------------------------------------------------*
 * Resume Instruction Execution in an Invalid Core                            *
 *----------------------------------------------------------------------------*/

/**
 * @brief Fault Injection Tests: Wakeup an Invalid Core
 */
PRIVATE void test_cluster_core_fault_wakeup_inval(void)
{
    KASSERT(core_wakeup(-1) == -EINVAL);
    KASSERT(core_wakeup(CORES_NUM) == -EINVAL);
}

/*----------------------------------------------------------------------------*
 * Resume Instruction Execution in a Bad Core                                 *
 *----------------------------------------------------------------------------*/

/**
 * @brief Fault Injection Tests: Wakeup a Valid Core in a Bad Context
 */
PRIVATE void test_cluster_core_fault_resume_bad(void)
{
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            KASSERT(core_wakeup(i) == -EINVAL);
            break;
        }
    }
}

/*============================================================================*
 * Stress Tests                                                               *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Start Execution from Master Core                                           *
 *----------------------------------------------------------------------------*/

/**
 * @brief Stress Test: Start Execution from Master Core
 */
PRIVATE void test_cluster_cores_stress_master_start(void)
{
    for (int k = 0; k < NITERATIONS; k++) {
        fence_init(&slave_fence, CORES_NUM - 1);

        /* Start execution in all cores. */
        for (int i = 0; i < CORES_NUM; i++) {
            if (i != COREID_MASTER) {
                int ret;

                do {
                    ret = core_start(i, slave);
                    KASSERT((ret == 0) || (ret == -EBUSY));
                } while (ret != 0);
            }
        }

        /* Wait for slave cores. */
        fence_wait(&slave_fence);
    }
}

/*----------------------------------------------------------------------------*
 * Start Execution from Leader Core                                           *
 *----------------------------------------------------------------------------*/

/**
 * @brief Stress Test: Start Execution from Leader Core
 */
PRIVATE void test_cluster_cores_stress_leader_start(void)
{
    /* Test not applicable. */
    if (CORES_NUM <= 2)
        return;

    nslaves = CORES_NUM - 2;
    dcache_invalidate();

    for (int k = 0; k < NITERATIONS; k++) {
        fence_init(&leader_fence, 1);

        /* Start execution in all cores. */
        for (int i = 0; i < CORES_NUM; i++) {
            if (i != COREID_MASTER) {
                int ret;

                do {
                    ret = core_start(i, leader);
                    KASSERT((ret == 0) || (ret == -EBUSY));
                } while (ret != 0);

                break;
            }
        }

        /* Wait for slave cores. */
        fence_wait(&leader_fence);
    }
}

/*----------------------------------------------------------------------------*
 * Spinlock Tests                                                             *
 *----------------------------------------------------------------------------*/

/**
 * @brief Producer and consumer semaphores
 */
struct semaphore mutex;
struct semaphore empty;
struct semaphore full;

#define BUFFER_SIZE 32  /* Buffer size.              */
#define ELEMENTS_LOGN 9 /* Elements in power of two. */

/**
 * @brief Given an N (power of two)
 * sums the sequence between 0 and (1 << N).
 */
#define SUM_POWER_OF_TWO(N) (((1 << ((N) + (N))) + (1 << (N))) >> 1)

PRIVATE int buffer[BUFFER_SIZE]; /* Producer/consumer buffer. */
PRIVATE int producer_sum;        /* Producer sum.             */
PRIVATE int consumer_sum;        /* Consumer sum.             */

/**
 * @brief Producer.
 */
PRIVATE void producer(void)
{
    int in;      /* Current position. */
    int counter; /* Producer counter. */
    int item;    /* Current item.     */

    producer_sum = 0;
    counter = 0;
    item = 0;
    in = 0;

    while (counter <= (1 << ELEMENTS_LOGN)) {
        semaphore_down(&empty);
        semaphore_down(&mutex);

        producer_sum += item;
        buffer[in] = item;
        item++;
        in = (in + 1) & (BUFFER_SIZE - 1);

        counter++;

        semaphore_up(&mutex);
        semaphore_up(&full);
    }
}

/**
 * @brief Consumer.
 */
PRIVATE void consumer(void)
{
    int out;     /* Current position. */
    int counter; /* Consumer counter. */
    int item;    /* Current item.     */

    consumer_sum = 0;
    counter = 0;
    out = 0;

    while (counter <= (1 << ELEMENTS_LOGN)) {
        semaphore_down(&full);
        semaphore_down(&mutex);

        item = buffer[out];
        consumer_sum += item;
        out = (out + 1) & (BUFFER_SIZE - 1);

        semaphore_up(&mutex);
        semaphore_up(&empty);
        counter++;
    }

    fence_join(&slave_fence);

    KASSERT(core_release() == 0);
    core_reset();
}

/**
 * @brief Stress Test: Spinlock Test
 */
PRIVATE void test_cluster_cores_stress_spinlocks(void)
{
    /* Initialize semaphores. */
    semaphore_init(&mutex, 1);
    semaphore_init(&empty, BUFFER_SIZE);
    semaphore_init(&full, 0);

    /* Start the producer/consumer. */
    fence_init(&slave_fence, 1);

    /* Start the first available slave core. */
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            KASSERT(core_start(i, consumer) == 0);
            break;
        }
    }

    producer();

    fence_wait(&slave_fence);

    /*
     * If the consumer sum is different from the expected
     * the master core will intentionally hang up in order
     * to indicate a wrong result.
     */
    if (SUM_POWER_OF_TWO(ELEMENTS_LOGN) != consumer_sum)
        while (true)
            ;
}

#if CORE_SUPPORTS_MULTITHREADING

/*----------------------------------------------------------------------------*
 * Context switch                                                             *
 *----------------------------------------------------------------------------*/

/**
 * @name Auxiliar variables.
 */
/**@{*/
PRIVATE struct context *level_0_ctx;
PRIVATE struct context *level_1_ctx;
PRIVATE struct context *level_2_ctx;
PRIVATE struct stack ustack1;
PRIVATE struct stack kstack1;
PRIVATE struct stack ustack2;
PRIVATE struct stack kstack2;
/**@}*/

/**
 * @name Context fences.
 */
/**@{*/
PRIVATE struct fence level_0_fence;
PRIVATE struct fence level_1_fence;
PRIVATE struct fence level_2_fence;
/**@}*/

/**
 * @brief API Test: Next context function.
 */
PRIVATE void context_level_2(void)
{

#if (TEST_CORES_VERBOSE)
    kprintf("[test][core] Level 2: New context started.");
#endif

    fence_join(&level_2_fence);

#if (TEST_CORES_VERBOSE)
    kprintf("[test][core] Level 2: Restore level 1 context.");
#endif

    KASSERT(context_switch_to(&level_2_ctx, &level_1_ctx) == 0);
}

/**
 * @brief API Test: Next context function.
 */
PRIVATE void context_level_1(void)
{
#if (TEST_CORES_VERBOSE)
    kprintf("[test][core] Level 1: New context started.");
    kprintf("[test][core] Level 1: Switch to level 2.");
#endif

    KASSERT(context_switch_to(&level_1_ctx, &level_2_ctx) == 0);

#if (TEST_CORES_VERBOSE)
    kprintf("[test][core] Level 1: Context restored back.");
#endif

    fence_join(&level_1_fence);

#if (TEST_CORES_VERBOSE)
    kprintf("[test][core] Level 1: Restore level 0 context.");
#endif

    KASSERT(context_switch_to(&level_1_ctx, &level_0_ctx) == 0);
}

/**
 * @brief API Test: Previous context function.
 */
PRIVATE void core_switch_context(void)
{
#if (TEST_CORES_VERBOSE)
    kprintf("[test][core] Level 0: Core prepare to switch context.");
#endif

    KASSERT(context_switch_to(&level_0_ctx, &level_1_ctx) == 0);

#if (TEST_CORES_VERBOSE)
    kprintf("[test][core] Level 0: Context restored back.");
#endif

    fence_join(&level_0_fence);
}

/**
 * @brief API Test: Restore a context.
 */
PRIVATE void test_cluster_core_restore_context(void)
{
    for (int t = 0; t < NITERATIONS; t++) {
        KASSERT((level_1_ctx = context_create(
                     context_level_1, &ustack1, &kstack1)) != NULL);
        KASSERT((level_2_ctx = context_create(
                     context_level_2, &ustack2, &kstack2)) != NULL);

        /* Start the producer/consumer. */
        fence_init(&level_0_fence, 1);
        fence_init(&level_1_fence, 1);
        fence_init(&level_2_fence, 1);

        /* Start the first available slave core. */
        for (int i = 0; i < CORES_NUM; i++) {
            if (i != COREID_MASTER) {
                KASSERT(core_start(i, core_switch_context) == 0);
                break;
            }
        }

#if (TEST_CORES_VERBOSE)
        kprintf("[test][core] Master: Wait level 2");
#endif
        fence_wait(&level_2_fence);

#if (TEST_CORES_VERBOSE)
        kprintf("[test][core] Master: Wait level 1");
#endif
        fence_wait(&level_1_fence);

#if (TEST_CORES_VERBOSE)
        kprintf("[test][core] Master: Wait level 0");
#endif
        fence_wait(&level_0_fence);

#if (TEST_CORES_VERBOSE)
        kprintf("[test][core] Master: Done");
#endif
    }
}

#endif /* CORE_SUPPORTS_MULTITHREADING */

#if CLUSTER_HAS_IPI

/*----------------------------------------------------------------------------*
 * IPI                                                                        *
 *----------------------------------------------------------------------------*/

/**
 * @brief IPI semaphore.
 */
PRIVATE struct semaphore ipi_sem;

/**
 * @brief Counter.
 */
PRIVATE volatile int ipi_counter ALIGN(CACHE_LINE_SIZE);
PRIVATE volatile int ipi_master ALIGN(CACHE_LINE_SIZE);
PRIVATE volatile int ipi_coreid ALIGN(CACHE_LINE_SIZE);

/**
 * @brief IPI Handler.
 */
PRIVATE void ipi_handler(void)
{
    KASSERT(ipi_coreid == core_get_id());

    if (ipi_counter == ipi_master)
        ipi_counter++;
    else
        kprintf("[test][cluster][event][ipi] Spurious interrupt!");
}

/**
 * @brief Interruptible rule.
 */
PRIVATE void interruptible(void)
{
    ipi_counter = 0;
    ipi_master = 0;

    event_drop();

    interrupts_enable();
    interrupt_mask(INTERRUPT_TIMER);

    semaphore_up(&ipi_sem);

    event_wait();

    /* Handler will release this while. */
    KASSERT(ipi_counter == 1);

    semaphore_up(&ipi_sem);

    event_wait();

    /* Handler will release this while. */
    KASSERT(ipi_counter == 2);

    interrupt_unmask(INTERRUPT_TIMER);
    interrupts_disable();

    semaphore_up(&ipi_sem);

    KASSERT(core_release() == 0);
    core_reset();
}

/**
 * @brief Interruptor rule.
 */
PRIVATE void interruptor(int coreid)
{

    /* Waits core wake up. */
    semaphore_down(&ipi_sem);

    event_notify(coreid);

    /* Intermediarie wait. */
    semaphore_down(&ipi_sem);

    ipi_master++;

    event_notify(coreid);

    /* Waits core finishes. */
    semaphore_down(&ipi_sem);

    ipi_master++;
}

/**
 * @brief Inter Core Interrupts
 */
PRIVATE void test_cluster_core_api_inter_core_interrupt(void)
{
    KASSERT(event_register_handler(ipi_handler) == 0);

    /* Start the first available slave core. */
    for (int i = 0; i < CORES_NUM; i++) {
        if (i != COREID_MASTER) {
            ipi_coreid = i;

            /* Start the producer/consumer. */
            semaphore_init(&ipi_sem, 0);

            KASSERT(core_start(i, interruptible) == 0);

            interruptor(i);
        }
    }

    KASSERT(event_unregister_handler() == 0);
}

#endif /* CLUSTER_HAS_IPI */

/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * @brief API Tests.
 */
PRIVATE struct test core_tests_api[] = {
    {test_cluster_core_api_start_slave, "start execution in a slave core    "},
#ifndef __unix64__
    {test_cluster_core_api_reset_slave, "reset slave a core                 "},
#endif /* !__unix64__ */
    {test_cluster_core_api_sleep_wakeup_slave,
     "suspend and resume a slave core    "},
    {test_cluster_core_api_start_leader, "start execution from slave         "},
    {test_cluster_core_api_sleep_wakeup_leader,
     "suspend and resume from slave core "},
#if CORE_SUPPORTS_MULTITHREADING
    {test_cluster_core_restore_context, "Create a context and restore it    "},
#endif
#if CLUSTER_HAS_IPI
    {test_cluster_core_api_inter_core_interrupt,
     "interrupt others cores             "},
#endif
    {NULL, NULL},
};

/**
 * @brief Fault Injection Tests.
 */
PRIVATE struct test fault_tests_api[] = {
    {test_cluster_core_fault_start_master,
     "start execution in the master core "},
    {test_cluster_core_fault_start_inval,
     "start an invalid execution flow    "},
    {test_cluster_core_fault_start_bad, "start a bad execution flow         "},
    {test_cluster_core_fault_reset_master,
     "reset execution in the master core "},
    {test_cluster_core_fault_wakeup_inval,
     "resume execution in an invalid core"},
    {test_cluster_core_fault_resume_bad, "resume execution in a bad core     "},
    {NULL, NULL},
};

/**
 * @brief Stress Injection Tests.
 */
#if defined(__ENABLE_STRESS_TESTS)
PRIVATE struct test stress_tests_api[] = {
    {test_cluster_cores_stress_master_start, "start from master core"},
    {test_cluster_cores_stress_leader_start, "start from leader core"},
    {test_cluster_cores_stress_spinlocks, "spinlock test         "},
    {NULL, NULL},
};
#endif

/**
 * The test_cluster_cores() function launches testing units on the
 * Cores Interface of the Cluster AL.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void test_cluster_cores(void)
{
    /* API Tests */
    CLUSTER_KPRINTF(HLINE);
    for (int i = 0; core_tests_api[i].test_fn != NULL; i++) {
        core_tests_api[i].test_fn();
        CLUSTER_KPRINTF("[test][cluster][cores][api] %s [passed]",
                        core_tests_api[i].name);
    }

    /* Fault Tests */
    CLUSTER_KPRINTF(HLINE);
    for (int i = 0; fault_tests_api[i].test_fn != NULL; i++) {
        fault_tests_api[i].test_fn();
        CLUSTER_KPRINTF("[test][cluster][cores][fault] %s [passed]",
                        fault_tests_api[i].name);
    }

    /* Stress Tests */
#if defined(__ENABLE_STRESS_TESTS)
    CLUSTER_KPRINTF(HLINE);
    for (int i = 0; stress_tests_api[i].test_fn != NULL; i++) {
        stress_tests_api[i].test_fn();
        CLUSTER_KPRINTF("[test][cluster][cores][stress] %s [passed]",
                        stress_tests_api[i].name);
    }
#endif
}
