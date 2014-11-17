#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

static int is_numeric(const char *s)
{
	char *p;

	if (s == NULL || *s == '\0' || isspace(*s))
		return 0;
	strtod(s, &p);

	return *p == '\0';
}

static int is_verbose(char *arg)
{
	return strlen(arg) == 2 && strncmp(arg, "-v", 2) == 0;
}

#define PAGE_SIZE			(4*1024)
#define PGD_TABLE_SIZE			(1024*PAGE_SIZE)
#define expose_page_table(a, b, c)	syscall(378, a, b, c)
/* TODO IMPLEMENT BELOW */
#define virt(vma)			0
#define phys(vma)			0
#define filebit(vma)			0
#define dirtybit(vma)			0
#define readonlybit(vma)		0
#define xnbit(vma)			0

int main(int argc, char **argv)
{
	int i;
	int fd;
	int ret;
	int pid = -1;
	int verbose;
//	unsigned long vma;
//	unsigned long *addr;
	unsigned long fake_pgd;
	unsigned long accessed;

	verbose = 0;

	if (argc != 2 && argc != 3) {
		printf("Usage:%s [-v] [pid]\n", argv[0]);
		return -1;
	}

	if (argc == 2) {
		if (is_numeric(argv[1])) {
			pid = atoi(argv[1]);
		} else {
			printf("Usage:%s [-v] [pid]\n", argv[0]);
			return -1;
		}
	} else if (argc == 3) {
		if (is_numeric(argv[1]) && is_verbose(argv[2])) {
			verbose = 1;
			pid = atoi(argv[1]);
		} else if (is_verbose(argv[1]) && is_numeric(argv[2])) {
			verbose = 1;
			pid = atoi(argv[2]);
		} else {
			printf("usage: %s [-v] [pid]\n", argv[0]);
			return -1;
		}
	}

	fd = open("/dev/zero", O_RDONLY);
	if (fd == -1) {
		close(fd);
		perror("open");
		exit(EXIT_FAILURE);
	}

	fake_pgd = (unsigned long) mmap(NULL, 2 * 4 * 1024 * 1024, PROT_READ, MAP_SHARED, fd, 0);
	if (fake_pgd == (unsigned long) NULL) {
		perror("mmap");
		close(fd);
		exit(EXIT_FAILURE);
	}
	close(fd);




	for (i = 0; i < PGD_TABLE_SIZE / sizeof(unsigned long); i++) {
		accessed = (unsigned long)((unsigned long *) fake_pgd + i);
	}
	(void) accessed;

	printf("fake_pgd:%p\n", (void *)fake_pgd);
	printf("addr:%p\n", (void *) (fake_pgd + PGD_TABLE_SIZE));

	ret = 0;
	printf("%ld %d %p\n", (long)pid, ret, (void *) (fake_pgd + PGD_TABLE_SIZE));
	
	ret = expose_page_table(pid, fake_pgd, fake_pgd + PGD_TABLE_SIZE);
	if (ret != 0) {
		perror("syscall: ");
		return ret;
	}
	(void) verbose;
//	for (i = 0; i < PGD_TABLE_SIZE; i++) {
//		vma = addr[i*4*1024]; /* TODO not sure if this is correct */
//		if (vma == 0)
//			printf("%p %p %p %d %d %d %d %d\n",
//				i, virt(vma), phys(vma),
//				filebit(vma), dirtybit(vma),
//				readonlybit(vma), xnbit(vma));
//		else if (verbose)
//			printf("%p %p 0 0 0 0 0 0", index(vma), virt(vma));
//	}

	return 0;
}
