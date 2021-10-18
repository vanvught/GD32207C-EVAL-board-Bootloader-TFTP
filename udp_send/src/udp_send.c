/*
 ============================================================================
 Name        : udp_send.c
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#define BUFLEN	512
#define PORT 	10501

void fail(char *s) {
	perror(s);
	exit(1);
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: %s ip_address\n", argv[0]);
		return -1;
	}

	int sockfd, n;
	struct sockaddr_in si_me;
	struct sockaddr_in servaddr;
	size_t slen = sizeof(servaddr);

	char message[BUFLEN];
	char buffer[BUFLEN];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		fail("socket");
	}

	struct timeval recv_timeout;
	recv_timeout.tv_sec = 1;
	recv_timeout.tv_usec = 0;

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&recv_timeout, sizeof(recv_timeout)) == -1) {
		fail("setsockopt(SO_RCVTIMEO)");
	}

	int val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1) {
		fail("setsockopt(SO_REUSEADDR)");
	}

	val = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val)) == -1) {
		fail("setsockopt(SO_REUSEPORT)");
	}

    memset(&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockfd, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
		fail("bind");
	}

	memset((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;

	if (inet_aton(argv[1], &servaddr.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	n = read(STDIN_FILENO, message, BUFLEN);

	if (sendto(sockfd, message, n, 0, (struct sockaddr*) &servaddr, slen) == -1) {
		close(sockfd);
		fail("sendto()");
	}

	if (message[0] == '?') {
		memset(buffer, '\0', BUFLEN);
		if ((n = recvfrom(sockfd, buffer, BUFLEN, MSG_WAITALL, (struct sockaddr*) &servaddr, (socklen_t *)&slen)) == -1) {
			if (1 && (errno != EAGAIN) && (errno != EWOULDBLOCK)) { // EAGAIN and EWOULDBLOCK can be equal
				close(sockfd);
				fail("recvfrom");
			}
		}

		buffer[n] = '\0';
		printf("%s", buffer);
	}

	close(sockfd);

	return EXIT_SUCCESS;
}
