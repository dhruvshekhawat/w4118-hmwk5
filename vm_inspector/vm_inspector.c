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

#define PAGE_SIZE			4096
#define PAGE_SHIFT			12
#define TWEAKED_PGD_ENTRIES		2048
#define TWEAKED_PTE_ENTRIES		512
#define TWEAKED_PTE_ENTRY_SIZE		4
#define EXPOSED_TBL_ENTRIES		(2048 * 512)

#define EXPOSED_TBL_SIZE 		( 2 * EXPOSED_TBL_ENTRIES \
					  * TWEAKED_PTE_ENTRY_SIZE)

#define pte_offset(index)		(index % 512)
#define pte_base(index)			(((index / 512) * PAGE_SIZE) / 4)
#define base_address(page)		(page * PAGE_SIZE)
#define pfn_of_pte(pte)			(pte >> PAGE_SHIFT)

#define filebit(vma)			((vma & (1 << 2)) > 0)
#define dirtybit(vma)			((vma & (1 << 6)) > 0)
#define readonlybit(vma)		((vma & (1 << 7)) > 0)
#define xnbit(vma)			0
#define pte_none(pte)			(!pte)

#define expose_page_table(a, b, c)	syscall(378, a, b, c)

/*
 * Helpers for argument parsing
 */
static int is_numeric(const char *s)
{
	char *p;

	if (s == NULL || *s == '\0' || isspace(*s))
		return 0;
	strtod(s, &p);

	return *p == '\0';
}

static inline int is_verbose(char *arg)
{
	return strlen(arg) == 2 && strncmp(arg, "-v", 2) == 0;
}

int main(int argc, char **argv)
{
	int i;
	int fd;
	int rval;
	int pid;
	int verbose;
	unsigned long fake_pgd;


	if (argc != 2 && argc != 3) {
		printf("Usage:%s [-v] pid\n", argv[0]);
		return -1;
	}
	pid = -1;
	verbose = 0;
	if (argc == 2) {
		/*
		 * If given one argument, check whether it is "-v" or
		 * a pid? If not, set pid equal to -1 in order to
		 * tract the current process.
		 */
		if (!is_numeric(argv[1])) {
			printf("usage: %s [-v] pid\n", argv[0]);
			return -1;
		}
		pid = atoi(argv[1]);
	} else if (argc == 3) {
		/*
		 * If given twho arguments, try to match vebose
		 * and pid. If fail, arguments are malformed.
		 */
		if (is_numeric(argv[1]) && is_verbose(argv[2])) {
			verbose = 1;
			pid = atoi(argv[1]);
		} else if (is_verbose(argv[1]) && is_numeric(argv[2])) {
			verbose = 1;
			pid = atoi(argv[2]);
		} else {
			printf("usage: %s [-v] pid\n", argv[0]);
			return -1;
		}
	}

	/*
	 * TODO: Remove this and use MAP_PRIVATE
	 */
	fd = open("/dev/zero", O_RDONLY);
	if (fd == -1) {
		close(fd);
		perror("open");
		return -1;
	}

	fake_pgd = (unsigned long) mmap(NULL,
					EXPOSED_TBL_SIZE,
					PROT_READ, MAP_SHARED, fd, 0);

	if (fake_pgd == (unsigned long) NULL) {
		perror("mmap");
		close(fd);
		return -1;
	}
	close(fd);

	rval = expose_page_table(pid, fake_pgd, fake_pgd);
	if (rval != 0) {
		perror("syscall: ");
		return rval;
	}

	for (i = 0; i < EXPOSED_TBL_ENTRIES; ++i) {

		unsigned int pte;

		if (((unsigned long *)fake_pgd)[pte_base(i) +
		     pte_offset(i)] != 0) {
			pte = ((unsigned long *)fake_pgd)[pte_base(i) +
				pte_offset(i)];
			printf("%d 0x%x 0x%x %d %d %d\n",
			       i, base_address(i),
			       pfn_of_pte(pte), filebit(pte),
			       dirtybit(pte), readonlybit(pte));
			continue;
		}
		if (verbose)
			printf("0 0x0 0x0 0 0 0\n");
	}

	munmap((void *)fake_pgd, EXPOSED_TBL_SIZE);

	return 0;
}
