#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define BACKLOG 5
#define BUFMAX 1024
#define PORT 21896

struct sumsqrt_t {
	int lower;
	int upper;
};

struct update_t {
	size_t count;
	double values[0];
};

enum ptype_t {
	UPDATE=0,
	SUMSQRT=1,
	MESSAGE=2,
	ERROR=3
};

struct header_t {
	enum ptype_t ptype;
	size_t len;
};

union body_t {
	struct sumsqrt_t sumsqrt;
	struct update_t update;
	char message[BUFMAX/4];
};

struct packet_t {
	struct header_t header;
	union body_t body;
};

int main(int, char * []);
void signal_handler(int);
double sumsqrt_range(int, int);
void update_list(size_t, double []);

void signal_handler(int sig) {
	if (sig == SIGCHLD) {
		wait(0);
	}
}

double sumsqrt_range(int lower, int upper) {
	if (lower > upper) {
		return 0;
	} else if (lower == upper) {
		return sqrt(lower);
	}

	double sum = 0;
	for (int i = lower; i <= upper; i++) {
		sum += sqrt(i);
	}
	return sum;
}

void update_list(size_t count, double values[count]) {
	for (size_t i = 0; i < count; i++) {
		values[i] = values[i] * values[i] / 10.0;
	}
}

int main(int argc, char * argv[]) {
	if (signal(SIGCHLD, signal_handler) < 0) {
		perror("signal error");
		return 1;
	}

	struct sockaddr_in local;
	socklen_t len=sizeof(local);
	int sk;
	char sendbuf[BUFMAX], recvbuf[BUFMAX];

	if ((sk = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		return 1;
	}

	local.sin_family = AF_INET;
	local.sin_port = htons(PORT);
	local.sin_addr.s_addr = INADDR_ANY;

	if (bind(sk, (struct sockaddr *)&local, len) < 0) {
		perror("bind error");
		close(sk);
		return 1;
	}

	if (listen(sk, BACKLOG) < 0) {
		perror("listen error");
		close(sk);
		return 1;
	}

	while (1) {
		int csk;
		if ((csk = accept(sk, NULL, NULL)) < 0) {
			perror("accept error");
			continue;
		}

		pid_t cpid = fork();
		if (cpid == 0) {
			close(sk);

			struct packet_t pkt;
			if (recv(csk, recvbuf, sizeof(struct packet_t), 0) < 0) {
				perror("recv error");
				close(csk);
				exit(1);
			}
			memcpy(&pkt, recvbuf, sizeof(struct packet_t));

			int ptype = ntohl(pkt.header.ptype);
			if (ptype == SUMSQRT) {
				int lower = ntohl(pkt.body.sumsqrt.lower);
				int upper = ntohl(pkt.body.sumsqrt.upper);
				double sum = sumsqrt_range(lower, upper);
				snprintf(
					pkt.body.message,
					sizeof(pkt.body.message),
					"%f%c", sum, '\0'
				);
			} else if (ptype == UPDATE) {
				size_t count = ntohl(pkt.body.update.count);
				for (size_t i = 0; i < count; i++) {
					int * ip = (int *)&pkt.body.update.values[i];
					*ip = ntohl(*ip);
				}
				update_list(count, pkt.body.update.values);
				for (size_t i = 0; i < count; i++) {
					int * ip = (int *)&pkt.body.update.values[i];
					*ip = htonl(*ip);
				}
			} else {
				snprintf(
					pkt.body.message, 
					sizeof(pkt.body.message),
					"Error: Invalid packet type specified!\n"
				);
			}

			memcpy(sendbuf, &pkt, sizeof(struct packet_t));
			if (send(csk, sendbuf, sizeof(struct packet_t), 0) < 0) {
				perror("send error");
				close(csk);
				exit(1);
			}

			close(csk);
			exit(0);
		} else if (cpid > 0) {
			close(csk);
		} else {
			perror("fork error");
			close(csk);
			continue;
		}
	}

	return 0;
}
