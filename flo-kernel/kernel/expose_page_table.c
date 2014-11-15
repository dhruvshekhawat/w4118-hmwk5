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
SYSCALL_DEFINE3(expose_page_table,
				pid_t, pid,
				unsigned long, fake_pgd,
				unsigned long, addr)
{

	int ret_code = 0;
	struct task_struct *tsk;
	struct mm_struct *tsk_mm;
	struct vm_area_struct *tsk_vma;
	struct mm_walk page_walk = {};

	if (addr & ~PAGE_MASK || fake_pgd & ~PAGE_MASK)
		return -EINVAL;

	if (pid == -1)
		tsk = find_task_by_vpid(current->pid);
	else
		tsk = find_task_by_vpid(pid);
	if (tsk == NULL)
		return -EINVAL;

	down_write(tsk_mm->mmap_sem);
	tsk_mm = tsk->>mm;
	vma = find_vma(tsk_mm,addr);
	vma->vm_flags |= VM_SPECIAL;
	page_walk.mm = mm;
	page_walk.private = vma;

	ret_code = walk_page_range(0, , &page_walk);

	return ret_code;

error:
	up_write(tsk_mm->mmap_sem);
	return ret_code;
}
