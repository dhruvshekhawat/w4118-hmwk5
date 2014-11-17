/*
 * kernel/expose_page_table.c
 * Copyright (C) 2014 V. Atlidakis, G. Koloventzos, A. Papancea
 *
 * COMS W4118 implementation of syscall for page table
 * exposure in user space for the current process.
 */
#include <asm-generic/errno-base.h>
#include <linux/syscalls.h>
#include <linux/major.h>
#include <asm/uaccess.h>
#include <asm/current.h>

#ifdef DEBUG
static int pte_debug_info(pte_t *pte, unsigned long addr, unsigned long end,
			  struct mm_walk *walk)
{
	/* print debug */
	return 0;
}
#endif

/* Helper to...
 *
 * @pmd:
 * @addr: starting address
 * @end:  ending address
 * @walk: set of callbacks to invoke for each level of the tree
 *
 */
static int remap_pte(pmd_t *pmd, unsigned long addr,
		     unsigned long end, struct mm_walk *walk)
{
	int rval;
	unsigned long pfn;
	unsigned long target;
	struct vm_area_struct *vma;

	vma = (struct vm_area_struct *)walk->private;
	pfn = page_to_pfn(pmd_page(*pmd));
	target = /* fetch  target */

	rval = 0;
	rval = remap_pfn_range(vma, target, pfn, PAGE_SIZE, vma->vm_page_prot);

	return rval;
}


/*
 * Map a target process's page table into address space of the current process.
 *
 * After successfully completing this call, addr will contain the
 * page tables of the target process. To make it efficient for referencing
 * the re-mapped page tables in user space, your syscall is asked to build a
 * fake pgd table. The fake pgd will be indexed by pgd_index(va) (i.e. index
 * for page directory for a given virtual address va).
 *
 * @pid: pid of the target process you want to investigate, if pid == -1,
 *       dump the current process's page tables
 * @fake_pgd: base address of the fake pgd table
 * @addr: base address in the user space that the page tables should map to
 *
 * NOTE: addr should be mmap'ed from user space at /dev/zero
 */
SYSCALL_DEFINE3(expose_page_table, pid_t, pid, unsigned long, fake_pgd,
		unsigned long, addr)
{
	int rval;
	struct inode *inode;
	struct task_struct *tsk;
	struct mm_struct *tsk_mm;
	struct vm_area_struct *tsk_vma;
	struct mm_walk walk_pte = {
		.mm = tsk->mm,
#ifdef DEBUG
		.pte_entry = pte_debug_info
#endif
		.pmd_entry = remap_pte,
		.pud_entry = remap_pgd,
	};

	if (addr & ~PAGE_MASK || fake_pgd & ~PAGE_MASK)
		return -EINVAL;

	tsk = pid == -1 ? current : find_task_by_vpid(pid);
	if (tsk == NULL)
		return -EINVAL;

	const unsigned long end_vaddr = TASK_SIZE_OF(tsk);

	tsk_mm = tsk->mm;

	/* find the first vma after addr */
	down_write(&tsk_mm->mmap_sem);
	tsk_vma = find_vma(tsk_mm, addr);
	if (tsk_vma == NULL) {
		rval = -EFAULT;
		goto error;
	}

	if (tsk_vma->vm_end - addr < PAGE_SIZE) {
		rval = -EINVAL;
		goto error;
	}

	if (tsk_vma->vm_flags & (VM_WRITE | VM_SPECIAL)) {
		rval = -EINVAL;
		goto error;
	}

	if (tsk_vma->vm_file) {
		inode = tsk_vma->vm_file->f_path.dentry->d_inode;
		/* Should we have more? This is for inode to be in /dev/zero*/
		if (imajor(inode) != MEM_MAJOR) {
			rval = -EINVAL;
			goto error;
		}
	} else {
		rval = -EINVAL;
		goto error;
	}

	walk_pte.private = tsk_vma;
	//rval = walk_page_range(0, , &walk_pte);

	rval = 0;
error:
	up_write(&tsk_mm->mmap_sem);
	return rval;
}
