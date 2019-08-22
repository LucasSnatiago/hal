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

#include <arch/processor/bostan/clusters.h>
#include <arch/processor/bostan/noc/dma.h>
#include <nanvix/const.h>
#include <errno.h>

/**
 * @brief Allocates and configures the control receiver buffer.
 *
 * @param interface Number of the DMA channel.
 * @param tag       Number of the control receiver buffer.
 * @param mask      Initial value of the buffer.
 *
 * @return Zero if create successfully and non zero otherwise.
 */
PUBLIC int bostan_dma_control_create(int interface, int tag, uint64_t mask, bostan_noc_handler_fn handler)
{
	if (bostan_cnoc_rx_alloc(interface, tag) != 0)
		return (-EBUSY);

	if (bostan_cnoc_rx_config(interface, tag, BOSTAN_CNOC_BARRIER_MODE, mask, handler) != 0)
	{
		bostan_cnoc_rx_free(interface, tag);
		return (-ECONNABORTED);
	}

	return (0);
}

/**
 * @brief Configure and send a signal on a control transfer buffer.
 *
 * @param interface    Number of the DMA channel.
 * @param tag          Number of the control transfer buffer.
 * @param target_nodes Target Node IDs.
 * @param ntargets     Amount of targets.
 * @param target_tag   Number of the target control receiver buffer.
 * @param mask         Signal value.
 *
 * @return Zero if send successfully and non zero otherwise.
 */
PUBLIC int bostan_dma_control_signal(
	int interface,
	int tag,
	const int *target_nodes,
	int ntargets,
	int target_tag,
	uint64_t mask
)
{
	int source_node;

	source_node = cluster_get_id() + interface;

	for (int i = 0; i < ntargets; i++)
	{
		if (bostan_cnoc_tx_config(interface, source_node, tag, target_nodes[i], target_tag))
			return (-EINVAL);

		if (bostan_cnoc_tx_write(interface, tag, mask) != 0)
			return (-EINVAL);
	}

	return (0);
}

/**
 * @brief Configure and async write to the target node.
 *
 * @param interface   Number of the DMA channel.
 * @param tag         Number of the data transfer buffer.
 * @param target_node Target Node ID.
 * @param target_tag  Target receiver buffer.
 * @param buffer      Local data pointer.
 * @param size        Amount of bytes to transfer.
 *
 * @return Zero if configure successfully and non zero otherwise.
 */
PUBLIC int bostan_dma_data_awrite(
	int interface,
	int tag,
	int target_node,
	int target_tag,
	const void *buffer,
	uint64_t size,
	uint64_t offset
)
{
	int source_node;

	source_node = cluster_get_id() + interface;

	return bostan_dnoc_uc_config_write(
		interface,
		source_node,
		tag,
		tag,
		target_node,
		target_tag,
		buffer,
		size,
		offset
	);
}

/**
 * @brief Configure and write to the target node.
 *
 * @param interface   Number of the DMA channel.
 * @param tag         Number of the data transfer buffer.
 * @param target_node Target Node ID.
 * @param target_tag  Target receiver buffer.
 * @param buffer      Local data pointer.
 * @param size        Amount of bytes to transfer.
 *
 * @return Zero if configure successfully and non zero otherwise.
 */
PUBLIC int bostan_dma_data_write(
	int interface,
	int tag,
	int target_node,
	int target_tag,
	const void *buffer,
	uint64_t size,
	uint64_t offset
)
{
	int source_node;

	source_node = cluster_get_id() + interface;

	if (bostan_dnoc_tx_config(interface, source_node, tag, target_node, target_tag) != 0)
		return (-EINVAL);

	return bostan_dnoc_tx_write(
		interface,
		tag,
		buffer,
		size,
		offset
	);
}