#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

static int isNumeric (const char * s)
{
	if (s == NULL || *s == '\0' || isspace(*s))
		return 0;
	char * p;
	strtod (s, &p);
	return *p == '\0';
}

int main(int argc, char **argv)
{
	bool verbose = false;
	int pid = -1;
	char *verbose ="-v";
	if (argc > 2) {
		printf("Usage:%s [-v] [pid]\n", argv[0]);
		return 1;
	}
	if (argc  == 1) {
		if (strcmp(argv[1],verbose) == 0) {
			verbose = true;
		} else if (isNumeric(argv[1])) {
			pid = atoi(argv[1]);
		} else {
			 printf("Usage:%s [-v] [pid]\n", argv[0]);
			 return 1;
		}
	} else if (argc == 2) {
		if (strcmp(argv[1],verbose) == 0 &&
		    isNumeric(argv[2])) {
			verbose = true;
			pid = atoi(argv[2]);
		} else if (strcmp(argv[2],verbose) == 0 &&
			   isNumeric(argv[2])) {
			verbose = true;
			 pid = atoi(argv[2]);
		} else
			printf("Usage:%s [-v] pid\n", argv[0]);
	}
	
	printf("shit\n");
	return 0;
}
