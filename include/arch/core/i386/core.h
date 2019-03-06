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

#ifndef ARCH_I386_CORE_H_
#define ARCH_I386_CORE_H_

/*============================================================================*
 *                               Core Interface                               *
 *============================================================================*/

/**
 * @addtogroup i386-core-core Core
 * @ingroup i386-core
 *
 * @brief i386 Core
 */
/**@{*/

	/**
	 * @name Size of Machine Types
	 */
	/**@{*/
	#define BYTE_SIZE  1 /**< Byte.        */
	#define WORD_SIZE  2 /**< Word.        */
	#define DWORD_SIZE 4 /**< Double word. */
	#define QWORD_SIZE 8 /**< Quad word.   */
	/**@}*/

	/**
	 * @name Bit-Length of Machine Types
	 */
	/**@{*/
	#define BYTE_BIT    8 /**< Byte.        */
	#define WORD_BIT   16 /**< Word.        */
	#define DWORD_BIT  32 /**< Double word. */
	#define QWORD_BIT  64 /**< Quad word.   */
	/**@}*/

#ifndef _ASM_FILE_

	#include <nanvix/const.h>
	#include <stdint.h>

	/**
	 * @name Machine Types
	 */
	/**@{*/
	typedef uint8_t byte_t;   /**< Byte.        */
	typedef uint16_t word_t;  /**< Word.        */
	typedef uint32_t dword_t; /**< Double word. */
	/**@}*/

	/**
	 * @brief Gets the ID of the core.
	 *
	 * The i386_core_get_id() returns the ID of the underlying core.
	 *
	 * @returns The ID of the underlying core.
	 */
	static inline int i386_core_get_id(void)
	{
		return (0);
	}

/**@}*/

/*============================================================================*
 * Provided Interface                                                         *
 *============================================================================*/

/**
 * @cond i386
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __core_get_id   /**< core_get_id()   */
	#define __core_shutdown /**< core_shutdown() */
	#define __core_sleep    /**< core_sleep()    */
	#define __core_wakeup   /**< core_wakeup()   */
	#define __core_start    /**< core_start()    */
	#define __core_reset    /**< core_reset()    */
	/**@}*/

	/**
	 * @see i386_core_get_id().
	 */
	static inline int core_get_id(void)
	{
		return (i386_core_get_id());
	}

/**@endcond*/

#endif /* _ASM_FILE_ */

#endif /* ARCH_I386_CORE_H_ */
