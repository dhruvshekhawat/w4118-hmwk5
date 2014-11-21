/*
 * kernel/expose_page_table.c
 * Copyright (C) 2014 V. Atlidakis, G. Koloventzos, A. Papancea
 *
 * COMS W4118 implementation of syscall for page table
 * exposure in user space for the current process.
 */

/*
 * The structural overview of fake pgd and remapped pte is the following
 * in which fake pdg indexes remmaped pte.
 *
 *   fake pdg          remapped pte
 * |     0     |----> +------------+ Remapped Address of Page Table 0
 * +-----------+      |            |
 * |     1     |---   |            |
 * +-----------+  |   +------------+
 * |     2     |  |   |     '      |
 * +-----------+  |   |     '      |
 * |     3     |  |   |     '      |
 * +-----------+  |   |     '      |
 * |     4     |  --> +------------+ Remapped Address of Page Table  1
 * +-----------+      |     '      |
 * |           |      |     '      |
 *       '                  '
 *       '                  '
 *
 * The size of the mmape'ed space must be sufficient to hold  4096
 * remapped pte ( 2048 * 4096 = 8 * 1024 * 1024 = 8 MB)
 *
 *
 *
 */
#define EXPOSED_TABLE_SIZE			(8 * 1024 * 1024)

/*
 * Use this struct to keep metadate while pagewalk invokes our
 * callback.
 *
 * @caller_vma: The caller's vma dedicated to expore the remaping
 * @addr: The current address when expose_pfn_range is exposeing
 *        remapped ptes.
 */
struct walk_metadata {
	struct vm_area_struct *caller_vma;
	unsigned long current_addr;
};
