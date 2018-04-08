#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
    /* create datagram socket */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        printf("failed to open socket; errno: %d\n", errno);
        return -1;
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(514);
    sa.sin_addr.s_addr = INADDR_ANY;

    /* bind socket to port */
    int r = bind(fd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in));
    if (r < 0) {
        printf("bind failed; errno: %d\n", errno);
        return -1;
    }

    // on each message
    while(1) {
	    char buffer[64 * 1024] = {0}; // 64kIB
	    struct sockaddr_storage src_addr;

	    int flags = 0;
	    ssize_t count = recvfrom(fd, &buffer, sizeof(buffer), flags, 0, 0);
	    if (count == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
		} else {
			// Parse the logging message
			printf("%s\n", buffer, count);
			fflush(stdout);
		}
    }
}
