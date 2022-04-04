#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int
main(int argc, char **argv)
{
        int fd, i;
        size_t nbytes;
        char buffer[1000];

        fd = open("/proc/csse332/status", O_RDWR);
        if (fd < 0) {
                fprintf(stderr, "Could not open your the procfs entry for reading: %s\n",
                        strerror(errno));
                exit(EXIT_FAILURE);
        }

        char buff[10];
	sprintf(buff, "R %d\0", getpid());
	int err = write(fd, buff, strlen(buff));
	sleep(10);
}
