/*
 * kernel/expose_page_table.c
 * Copyright (C) 2014 V. Atlidakis, G. Koloventzos, A. Papancea
 *
 * COMS W4118 implementation of syscall for page table
 * exposure in user space for the current process.
 */
#include <asm-generic/errno-base.h>
#include <linux/syscalls.h>
#include <asm/uaccess.h>
#include <asm/current.h>


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

}


/* Map a target process's page table into address space of the current process.
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
 */
SYSCALL_DEFINE3(expose_page_table, pid_t, pid, unsigned long, fake_pgd,
		unsigned long, addr)
{
	int ret_code = 0;
	struct task_struct *tsk;
	struct mm_struct *tsk_mm;
	struct vm_area_struct *tsk_vma;

	struct mm_walk walk = {
		.mm = tsk->mm,
		.pde_entry = remap_pte,
		.private = vma,
	};

	if (addr & ~PAGE_MASK || fake_pgd & ~PAGE_MASK)
		return -EINVAL;

	tsk = pid == -1 ? current : find_task_by_vpid(pid)
	if (tsk == NULL)
		return -EINVAL;
	
	down_write(tsk_mm->mmap_sem);
	tsk_mm = tsk->mm;
	vma = find_vma(tsk_mm,addr);
	if (vma == NULL) {
		ret_code = -EFAULT;
		goto error;
	}

	vma->vm_flags |= VM_SPECIAL;
	
	ret_code = walk_page_range(0, , &remap_pte);

error:
	up_write(&tsk_mm->mmap_sem);
	return ret_code;
}
