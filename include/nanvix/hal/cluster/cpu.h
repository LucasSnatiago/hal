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

#ifndef NANVIX_HAL_CLUSTER_CPU_H_
#define NANVIX_HAL_CLUSTER_CPU_H_

	/* Cluster Interface Implementation */
	#include <nanvix/hal/cluster/_cluster.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Constants */
	#ifndef HAL_NUM_CORES
	#error "HAL_NUM_CORES not defined"
	#endif

	/* Functions */
	#ifndef __hal_cpu_get_num_cores
	#error "hal_cpu_get_num_cores() not defined?"
	#endif

/*============================================================================*
 * CPU Interface                                                              *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-cluster-cpu CPU
 * @ingroup kernel-hal-cluster
 *
 * @brief CPU HAL Interface
 */
/**@{*/

	#include <nanvix/const.h>

	#if (HAL_NUM_CORES > 1)
	#define HAL_SMP
	#endif

	/**
	 * @brief Gets the number of cores.
	 *
	 * @returns The number of cores in the underlying processor.
	 */
	EXTERN int hal_cpu_get_num_cores(void);

/**@}*/

#endif /* NANVIX_HAL_CLUSTER_CPU_H_ */
