Exposing (remapping) process pages tablea into user space on ARM.
==
Copyright (C) 2014 V. Atlidakis, G. Koloventzos, A. Papancea

COMS-W4118 Columbia University

## MODIFIED/ADDED FILES:

- flo-kernel/arch/arm/include/asm/unistd.h
  defined expose_page_table syscall at position 378

- flo-kernel/arch/arm/kernel/calls.S
  added expose_page_table to syscall table at position 378

- flo-kernel/include/linux/syscalls.h
  defined the syscall expose_page_table

- flo-kernel/kernel/expose_page_table.c
  expose_page_table syscall implementation

- flo-kernel/kernel/Makefile
  added expose_page_table.o binary to kernel Makefile

- vm_inspector/vm_inspector.c
  the test program that prints out the fake pgd of the given pid

## RESOURCES USED:

1. Linux Cross Reference
   http://lxr.free-electrons.com

2. Linux Kernel Development (3rd Edition)

3. Operating Systems: Principles and Practice (2nd Edition)

4. Manual page for mmap
