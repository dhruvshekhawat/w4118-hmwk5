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

#define expose_page_table(a, b, c) syscall(378, a, b, c)

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

int main(int argc, char **argv)
{
	int fd;
	int ret;
	int pid = -1;
	int vflag = 0;
	unsigned long *addr;
	unsigned long *fake_pgd;

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
			vflag = 1;
			pid = atoi(argv[1]);
		} else if (is_verbose(argv[1]) && is_numeric(argv[2])) {
			vflag = 1;
			pid = atoi(argv[2]);
		} else {
			printf("usage: %s [-v] [pid]\n", argv[0]);
			return -1;
		}
	}

	fd = open("/dev/zero", O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	addr = mmap(NULL, 1024, PROT_READ, MAP_PRIVATE, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	printf("%ld\n", addr);

	ret = expose_page_table(pid, fake_pgd, addr);
	if (ret != 0) {
		perror("expose_page_table");
		return ret;
	}

	return 0;
}
