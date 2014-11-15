#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

int main(int argc, char **argv)
{
	int pid = -1;
	int vflag = 0;

	if (argc != 2 && argc != 3) {
		printf("Usage:%s [-v] [pid]\n", argv[0]);
		return 1;
	}

	if (argc == 2) {
		if (is_numeric(argv[1])) {
			pid = atoi(argv[1]);
		} else {
			 printf("Usage:%s [-v] [pid]\n", argv[0]);
			 return 1;
		}
	} else if (argc == 3) {
		if (is_numeric(argv[1]) && is_verbose(argv[2])) {
			vflag = 1;
			pid = atoi(argv[1]);
		} else if (is_verbose(argv[1]) && is_numeric(argv[2])) {
			vflag = 1;
			pid = atoi(argv[1]);
		} else {
			printf("usage: %s [-v] [pid]\n", argv[0]);
			return -1;
		}

		printf("verbose\n");
	}

	printf("shit\n");
	return 0;
}
