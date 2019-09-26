/* (c) 2019 - Steve Schnepp <steve.schnepp@pwkf.org> - Apache License 2.0 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>

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

    /* Drop privileges */
    if (geteuid() == 0) {
	    struct passwd *pw = getpwnam("nobody");
	    if (! pw) {
		    printf("getpwnam failed; errno: %d\n", errno);
		    return -2;
	    }

	    if ( setgid(pw->pw_gid) == -1 ) {
		    printf("setgid failed; errno: %d\n", errno);
		    return -3;
	    }

	    if ( setuid(pw->pw_uid) == -1 ) {
		    printf("setgid failed; errno: %d\n", errno);
		    return -4;
	    }
    }

    // on each message
    while(1) {
	    char buffer[64 * 1024] = {0}; // 64kIB
	    struct sockaddr_in src_addr;
	    socklen_t src_addr_len = sizeof(src_addr);

	    int flags = 0;
	    ssize_t count = recvfrom(fd, &buffer, sizeof(buffer) - 1, flags, (struct sockaddr*) &src_addr, &src_addr_len);
	    if (count == -1) {
		fprintf(stderr, "%s\n", strerror(errno));
		} else {
			// NUL-close the string
			buffer[count] = '\0';

			// Parse the logging message
			// A typical message recieved is "<12>cron: this is some message"
			int priority = LOG_NOTICE;
			syslog(priority, "[%s:%d] %s", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port), buffer);
		}
    }
}
