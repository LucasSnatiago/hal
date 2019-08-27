/*
 * MIT License
 *
 * Copyright (c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.  THE SOFTWARE IS PROVIDED
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* Must come fist. */
#define __NEED_PROCESSOR_LINUX64

#include <arch/processor/linux64.h>
#include <arch/target/unix64/unix64/mailbox.h>
#include <nanvix/hal/resource.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

/**
 * @brief Length of mailbox name.
 */
#define UNIX64_MAILBOX_NAME_LENGTH 128

/**
 * @brief Mailbox.
 */
struct mailbox
{
	struct resource resource;                  /**< Underlying resource.        */
	mqd_t fd;                                  /**< Underlying file descriptor. */
	char pathname[UNIX64_MAILBOX_NAME_LENGTH]; /**< Name of underlying mqueue.  */
	int nodeid;                                /**< ID of underlying node.      */
	int refcount;                              /**< Reference counter.          */
};

/**
 * @brief Table of mailboxes.
 */
PRIVATE struct
{
	/**
	 * @brief Input mailboxes.
	 */
	struct mailbox rxs[UNIX64_MAILBOX_CREATE_MAX];

	/**
	 * @brief Output mailboxes.
	 */
	struct mailbox txs[UNIX64_MAILBOX_OPEN_MAX];
} mailboxtab = {
	.rxs[0 ... UNIX64_MAILBOX_CREATE_MAX - 1] = {
		.resource = {0},
	},

	.txs[0 ... UNIX64_MAILBOX_OPEN_MAX - 1] = {
		.resource = {0},
	},
};

/**
 * @brief Resource pool for mailboxes.
 */
PRIVATE struct
{
	const struct resource_pool rx;
	const struct resource_pool tx;
} pool = {
	.rx = {mailboxtab.rxs, UNIX64_MAILBOX_CREATE_MAX, sizeof(struct mailbox)},
	.tx = {mailboxtab.txs, UNIX64_MAILBOX_OPEN_MAX,   sizeof(struct mailbox)},
};

/**
 * @brief Sync module lock.
 */
PRIVATE pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Default message queue attribute.
 */
PRIVATE struct mq_attr mq_attr = {
	.mq_maxmsg = LINUX64_PROCESSOR_NOC_NODES_NUM,
	.mq_msgsize = UNIX64_MAILBOX_MSG_SIZE
};

/*============================================================================*
 * unix64_mailbox_lock()                                                      *
 *============================================================================*/

/**
 * @brief Locks Unix mailbox module.
 */
PRIVATE void unix64_mailbox_lock(void)
{
	pthread_mutex_lock(&lock);
}

/*============================================================================*
 * unix64_mailbox_unlock()                                                    *
 *============================================================================*/

/**
 * @brief Unlocks Unix mailbox module.
 */
PRIVATE void unix64_mailbox_unlock(void)
{
	pthread_mutex_unlock(&lock);
}

/*============================================================================*
 * unix64_mailbox_rx_is_valid()                                               *
 *============================================================================*/

/**
 * @brief Asserts if an input mailbox is valid.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @returns One if the target mailbox is valid, and false otherwise.
 *
 * @note This function is non-blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int unix64_mailbox_rx_is_valid(int mbxid)
{
	return ((mbxid >= 0) && (mbxid < UNIX64_MAILBOX_CREATE_MAX));
}

/*============================================================================*
 * unix64_mailbox_tx_is_valid()                                               *
 *============================================================================*/

/**
 * @brief Asserts if an output mailbox is valid.
 *
 * @param mbxid ID of the target mailbox.
 *
 * @returns One if the target mailbox is valid, and false otherwise.
 *
 * @note This function is non-blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int unix64_mailbox_tx_is_valid(int mbxid)
{
	return ((mbxid >= 0) && (mbxid < UNIX64_MAILBOX_OPEN_MAX));
}

/*============================================================================*
 * unix64_mailbox_create()                                                    *
 *============================================================================*/

/**
 * @brief See unix64_mailbox_create().
 */
PRIVATE int do_unix64_mailbox_create(int nodenum)
{
	int mbxid;      /* Mailbox ID.         */
	int fd;         /* NoC connector.      */
	char *pathname; /* NoC connector name. */

	/* Check if input mailbox was already created. */
	for (int i = 0; i < UNIX64_MAILBOX_MAX; i++)
	{
		/* Skip invalid entries. */
		if (!resource_is_used(&mailboxtab.rxs[i].resource))
			continue;

		/* Found. */
		if (mailboxtab.rxs[i].nodeid == nodenum)
			return (-EEXIST);
	}

	/* Allocate a mailbox. */
	if ((mbxid = resource_alloc(&pool.rx)) < 0)
		goto error0;

	pathname = mailboxtab.rxs[mbxid].pathname;

	/* Build pathname for NoC connector. */
	sprintf(pathname,
		"/mailbox-%d",
		nodenum
	);

	/* Open NoC connector. */
	if ((fd = mq_open(pathname, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, &mq_attr)) == -1)
		goto error1;

	/* Initialize mailbox. */
	mailboxtab.rxs[mbxid].fd = fd;
	mailboxtab.rxs[mbxid].nodeid = nodenum;
	mailboxtab.rxs[mbxid].refcount = 1;
	resource_set_rdonly(&mailboxtab.rxs[mbxid].resource);
	resource_set_notbusy(&mailboxtab.rxs[mbxid].resource);

	return (mbxid);

error1:
	resource_free(&pool.rx, mbxid);
error0:
	return (-EAGAIN);
}

/**
 * @todo TODO: provide a detailed description for this function.
 *
 * @note This function is blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PUBLIC int unix64_mailbox_create(int nodenum)
{
	int mbxid;

	/* Invalid NoC node. */
	if ((nodenum < 0) || (nodenum >= PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Bad NoC node. */
	if (nodenum != linux64_processor_node_get_num(linux64_processor_node_get_id()))
		return (-EINVAL);

	unix64_mailbox_lock();
		mbxid = do_unix64_mailbox_create(nodenum);
	unix64_mailbox_unlock();

	return (mbxid);
}

/*============================================================================*
 * unix64_mailbox_open()                                                      *
 *============================================================================*/

/**
 * @brief See unix64_mailbox_open().
 */
PRIVATE int do_unix64_mailbox_open(int nodenum)
{
	int mbxid;      /* Mailbox ID.         */
	int fd;         /* NoC connector.      */
	char *pathname; /* NoC connector name. */

	/* Allocate a mailbox. */
	if ((mbxid = resource_alloc(&pool.tx)) < 0)
		goto error0;

	pathname = mailboxtab.txs[mbxid].pathname;

	/* Build pathname for NoC connector. */
	sprintf(pathname,
		"/mailbox-%d",
		nodenum
	);

	/* Open NoC connector. */
	if ((fd = mq_open(pathname, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR, &mq_attr)) == -1)
		goto error1;

	/* Initialize mailbox. */
	mailboxtab.txs[mbxid].fd = fd;
	mailboxtab.txs[mbxid].nodeid = nodenum;
	mailboxtab.txs[mbxid].refcount = 1;
	resource_set_wronly(&mailboxtab.txs[mbxid].resource);
	resource_set_notbusy(&mailboxtab.txs[mbxid].resource);

	return (mbxid);

error1:
	resource_free(&pool.tx, mbxid);
error0:
	return (-EAGAIN);
}

/**
 * @todo TODO: provide a detailed description for this function.
 *
 * @note This function is blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PUBLIC int unix64_mailbox_open(int nodenum)
{
	int mbxid;

	/* Invalid NoC node. */
	if ((nodenum < 0) || (nodenum >= PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Bad NoC node. */
	if (nodenum == linux64_processor_node_get_num(linux64_processor_node_get_id()))
		return (-EINVAL);

again:

	unix64_mailbox_lock();

	/*
	 * Check if we should just duplicate
	 * the underlying file descriptor.
	 */
	for (int i = 0; i < UNIX64_MAILBOX_MAX; i++)
	{
		/* Skip unused mailboxes. */
		if (!resource_is_used(&mailboxtab.txs[i].resource))
			continue;

		/* Not this node ID. */
		if (nodenum != mailboxtab.txs[i].nodeid)
			continue;

		/*
		 * Found, but mailbox is busy
		 * We have to wait a bit more.
		 */
		if (resource_is_busy(&mailboxtab.txs[i].resource))
		{
			unix64_mailbox_unlock();
			goto again;
		}

		mbxid = i;
		mailboxtab.txs[i].refcount++;
		goto out;
	}

	mbxid = do_unix64_mailbox_open(nodenum);

out:
	unix64_mailbox_unlock();
	return (mbxid);
}

/*============================================================================*
 * unix64_mailbox_unlink()                                                    *
 *============================================================================*/

/**
 * @todo TODO: provide a detailed description for this function.
 *
 * @note This function is blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int do_unix64_mailbox_unlink(int mbxid)
{
	int err;

again:

	unix64_mailbox_lock();

		/* Bad mailbox. */
		if (!resource_is_used(&mailboxtab.rxs[mbxid].resource))
		{
			err = -EINVAL;
			goto error1;
		}

		/* Busy mailbox. */
		if (resource_is_busy(&mailboxtab.rxs[mbxid].resource))
		{
			unix64_mailbox_unlock();
			goto again;
		}

		/* Destroy underlying message queue. */
		if (mq_close(mailboxtab.rxs[mbxid].fd) < 0)
		{
			err = -EAGAIN;
			goto error1;
		}
		mq_unlink(mailboxtab.rxs[mbxid].pathname);

		resource_free(&pool.rx, mbxid);

	unix64_mailbox_unlock();

	return (0);

error1:
	unix64_mailbox_unlock();
	return (err);
}

/**
 * @see do_unix64_mailbox_unlink().
 */
PUBLIC int unix64_mailbox_unlink(int mbxid)
{
	/* Invalid mailbox. */
	if (!unix64_mailbox_rx_is_valid(mbxid))
		return (-EINVAL);

	return (do_unix64_mailbox_unlink(mbxid));
}

/*============================================================================*
 * unix64_mailbox_close()                                                     *
 *============================================================================*/

/**
 * @todo TODO: provide a detailed description for this function.
 *
 * @note This function is blocking.
 * @note This function is thread-safe.
 * @note This function is reentrant.
 */
PRIVATE int do_unix64_mailbox_close(int mbxid)
{
	int err;

again:

	unix64_mailbox_lock();

		/* Bad mailbox. */
		if (!resource_is_used(&mailboxtab.txs[mbxid].resource))
		{
			err = -EINVAL;
			goto error1;
		}

		/* Busy mailbox. */
		if (resource_is_busy(&mailboxtab.txs[mbxid].resource))
		{
			unix64_mailbox_unlock();
			goto again;
		}

		/*
		 * Decrement reference counter and release
		 * the underlying file descriptor if we can.
		 */
		if (mailboxtab.txs[mbxid].refcount-- == 1)
		{
			/* Set mailbox as busy. */
			resource_set_busy(&mailboxtab.txs[mbxid].resource);

			/* Release lock, since we may sleep below. */
			unix64_mailbox_unlock();

			if (mq_close(mailboxtab.txs[mbxid].fd) < 0)
			{
				err = -EAGAIN;
				goto error2;
			}

			/* Re-acquire lock. */
			unix64_mailbox_lock();

			resource_free(&pool.tx, mbxid);
		}

	unix64_mailbox_unlock();

	return (0);

error2:
	unix64_mailbox_lock();
		resource_set_notbusy(&mailboxtab.txs[mbxid].resource);
error1:
	unix64_mailbox_unlock();
	return (err);
}

/**
 * @see do_unix64_mailbox_close().
 */
PUBLIC int unix64_mailbox_close(int mbxid)
{
	/* Invalid mailbox. */
	if (!unix64_mailbox_tx_is_valid(mbxid))
		return (-EINVAL);

	return (do_unix64_mailbox_close(mbxid));
}

/*============================================================================*
 * unix64_mailbox_awrite()                                                    *
 *============================================================================*/

/**
 * @todo TODO: provide a detailed description for this function.
 *
 * @note This function is thread-safe.
 */
PRIVATE ssize_t do_unix64_mailbox_awrite(int mbxid, const void *buf, size_t n)
{
again:

	unix64_mailbox_lock();

		/* Bad mailbox. */
		if (!resource_is_used(&mailboxtab.txs[mbxid].resource))
			goto error1;

		/* Busy mailbox. */
		if (resource_is_busy(&mailboxtab.txs[mbxid].resource))
		{
			unix64_mailbox_unlock();
			goto again;
		}

		/* Set mailbox as busy. */
		resource_set_busy(&mailboxtab.txs[mbxid].resource);

	/*
	 * Release lock, since we may sleep below.
	 */
	unix64_mailbox_unlock();

	if (mq_send(mailboxtab.txs[mbxid].fd, buf, n, 1) == -1)
		goto error2;

	unix64_mailbox_lock();
		resource_set_notbusy(&mailboxtab.txs[mbxid].resource);
	unix64_mailbox_unlock();

	return (n);

error2:
	unix64_mailbox_lock();
		resource_set_notbusy(&mailboxtab.txs[mbxid].resource);
	unix64_mailbox_unlock();
error1:
	unix64_mailbox_unlock();
	return (-EAGAIN);
}

/**
 * @see do_unix64_mailbox_awrite().
 */
PUBLIC ssize_t unix64_mailbox_awrite(int mbxid, const void *buf, size_t n)
{
	/* Invalid mailbox. */
	if (!unix64_mailbox_tx_is_valid(mbxid))
		return (-EINVAL);

	/* Invalid buffer. */
	if (buf == NULL)
		return (-EINVAL);

	/* Invalid write size. */
	if (n != UNIX64_MAILBOX_MSG_SIZE)
		return (-EINVAL);

	return (do_unix64_mailbox_awrite(mbxid, buf, n));
}

/*============================================================================*
 * unix64_mailbox_aread()                                                     *
 *============================================================================*/

/**
 * @todo TODO: provide a detailed description for this function.
 *
 * @note This function is thread-safe.
 */
PRIVATE ssize_t do_unix64_mailbox_aread(int mbxid, void *buf, size_t n)
{
	ssize_t nread;

again:

	unix64_mailbox_lock();

		/* Bad mailbox. */
		if (!resource_is_used(&mailboxtab.rxs[mbxid].resource))
			goto error1;

		/* Busy mailbox. */
		if (resource_is_busy(&mailboxtab.rxs[mbxid].resource))
		{
			unix64_mailbox_unlock();
			goto again;
		}

		/* Set mailbox as busy. */
		resource_set_busy(&mailboxtab.rxs[mbxid].resource);

	/*
	 * Release lock, since we may sleep below.
	 */
	unix64_mailbox_unlock();
	if ((nread = mq_receive(mailboxtab.rxs[mbxid].fd, buf, n, NULL)) == -1)
		goto error2;

	unix64_mailbox_lock();
		resource_set_notbusy(&mailboxtab.rxs[mbxid].resource);
	unix64_mailbox_unlock();

	return (nread);

error2:
	unix64_mailbox_lock();
		resource_set_notbusy(&mailboxtab.rxs[mbxid].resource);
	unix64_mailbox_unlock();
error1:
	unix64_mailbox_unlock();
	return (-EAGAIN);
}

/**
 * @see do_unix64_mailbox_aread().
 */
PUBLIC ssize_t unix64_mailbox_aread(int mbxid, void *buf, size_t n)
{
	/* Invalid mailbox. */
	if (!unix64_mailbox_rx_is_valid(mbxid))
		return (-EINVAL);

	/* Invalid buffer. */
	if (buf == NULL)
		return (-EINVAL);

	/* Invalid read size. */
	if (n != UNIX64_MAILBOX_MSG_SIZE)
		return (-EINVAL);

	return (do_unix64_mailbox_aread(mbxid, buf, n));
}
