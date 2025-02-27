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

#ifndef MBOOT_H_
#define MBOOT_H_

/* Multiboot header magic number. */
#define MBOOT_MAGIC 0x1badb002

/* Multiboot header flags. */
#define MBOOT_PAGE_ALIGN 0x00000001  /* Align modules on page boundary. */
#define MBOOT_MEMORY_INFO 0x00000002 /* Pass memory information.        */
#define MBOOT_VIDEO_MODE 0X00000004  /* Pass video information.         */
#define MBOOT_AOUT_KLUDGE 0x00010000 /* Pass aout information.          */

/* Multiboot information flags. */
#define MBOOT_INFO_MEMORY 0x00000001       /* Pass memory information.   */
#define MBOOT_INFO_DEV 0x00000002          /* Boot device set?           */
#define MBOOT_INFO_CMDLINE 0x00000004      /* Command-line defined?      */
#define MBOOT_INFO_MODS 0x00000008         /* Modules loaded?            */
#define MBOOT_INFO_AOUT_SYMS 0x00000010    /* a.out Symbol table loaded? */
#define MBOOT_INFO_ELF_SHDR 0x00000020     /* ELF section header table?  */
#define MBOOT_INFO_MEM_MAP 0x00000040      /* Memory map available?      */
#define MBOOT_INFO_DRIVE_INFO 0x00000080   /* Drive information?         */
#define MBOOT_INFO_CONFIG_TABLE 0x00000100 /* Configure table?           */
#define MBOOT_INFO_BOOT_LOADER 0x00000200  /* Bootloader name?           */
#define MBOOT_INFO_APM_TABLE 0x00000400    /* APM table available?       */
#define MBOOT_INFO_VIDEO_INFO 0x00000800   /* Video information?         */

/* Multiboot memory map entry type. */
#define MBOOT_MEMORY_AVAILABLE 1 /* Memory available. */
#define MBOOT_MEMORY_RESERVED 2  /* Memory reserved.  */

#ifndef _ASM_FILE_

#include <posix/stdint.h>

/*
 * Multiboot header.
 */
struct mboot_header {
    uint32_t magic;    /* Magic number.      */
    uint32_t flags;    /* Flags (see above). */
    uint32_t checksum; /* Checksum.          */

    /* a.out information. */
    uint32_t header_addr;   /* Header address.   */
    uint32_t load_addr;     /* Load address.     */
    uint32_t load_end_addr; /* Load end address. */
    uint32_t bss_end_addr;  /* Bss end address.  */
    uint32_t entry_addr;    /* Entry address.    */

    /* Video information. */
    uint32_t mode_type; /* Video mode type. */
    uint32_t width;     /* Video width.     */
    uint32_t height;    /* Video height.    */
    uint32_t depth;     /* Video depth.     */
};

/* a.out symbol table. */
struct aout_symb_table {
    uint32_t tabsize;
    uint32_t strsize;
    uint32_t addr;
    uint32_t reserved;
};

/* ELF section header table. */
struct elf_shhdr_table {
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
};

/*
 * Multiboot information.
 */
struct mboot_info {
    uint32_t flags; /* Flags (see above). */

    /* Available memory from BIOS. */
    uint32_t mem_lower; /* Lower memory available. */
    uint32_t mem_upper; /* Upper memory available. */

    uint32_t boot_device; /* Root partition. */
    uint32_t cmdline;     /* Command line; */

    /* Boot module list. */
    uint32_t mods_count; /* Modules count.       */
    uint32_t mods_addr;  /* Module list address. */

    union {
        struct aout_symb_table aout_symb; /* a.out symbol table.       */
        struct elf_shhdr_table elf_shhdr; /* ELF section header table. */
    } u;

    /* Memory mapping buffer. */
    uint32_t mmap_length; /* Length.   */
    uint32_t mmap_addr;   /* Adddress. */

    /* Drive information buffer. */
    uint32_t drives_length; /* Length. */
    uint32_t drives_addr;   /* Address. */

    uint32_t config_table;     /* ROM configuration table. */
    uint32_t boot_loader_name; /* Boot loader name.        */
    uint32_t apm_table;        /* APM table.               */

    /* Video. */
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
};

/*
 * Multiboot memory map entry.
 */
struct mboot_mmap_entry {
    uint32_t size; /* Size.             */
    uint64_t addr; /* Address.          */
    uint64_t len;  /* Length.           */
    uint32_t type; /* Type (see above). */
};

/*
 * Multiboot module info.
 */
struct mboot_mod_info {
    uint32_t mod_start; /* Start address.       */
    uint32_t mod_end;   /* End address.         */
    uint32_t cmdline;   /* Command line.        */
    uint32_t pad;       /* Padding to 16 bytes. */
};

#endif /* _ASM_FILE_*/

#endif /* MBOOT_H_ */
