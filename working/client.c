#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFMAX 1024
#define CLIENT_PORT 7777
#define SERVER_PORT 21896

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

int construct_pkt(int, char * [], struct packet_t *);
int get_server_addr(char *, struct in_addr *);
int main(int, char * []);

int construct_pkt(
		int argc, char * argv[], struct packet_t * pkt) {
	if (strcmp(argv[2], "-r") == 0) {
		pkt->header.ptype = htonl(SUMSQRT);
		pkt->body.sumsqrt.lower = htonl(atoi(argv[3]));
		pkt->body.sumsqrt.upper = htonl(atoi(argv[4]));
	} else if (strcmp(argv[2], "-u") == 0) {
		pkt->header.ptype = htonl(UPDATE);
		double values[argc-3];
		for (int i=0, j=3; i < argc-3 && j < argc; i++, j++) {
			double value = strtod(argv[j], NULL);
			values[i] = value;
			int * ip = (int *)&values[i];
			*ip = htonl(*ip);
		}
		memcpy(pkt->body.update.values, values, sizeof(values));
		pkt->body.update.count = htonl(sizeof(values)/sizeof(values[0]));
	} else {
		return -1;
	}

	return 0;
}

int get_server_addr(char * server, struct in_addr * dest) {
	struct hostent * hentry = gethostbyname(server);
	if (hentry == NULL) {
		return -1;
	}
	memcpy(dest, hentry->h_addr_list[0], sizeof(struct in_addr));
	return 0;
}

int main(int argc, char * argv[]) {
	// commands come from argc for this implementation
	if (argc < 4) {
		fputs("Error: Not enough arguments supplied!\n", stderr);
	}
	
	char * server = argv[1];
	char * cmd = argv[2];

	// sumsqrt_range function
	if (!(strcmp(cmd, "-r") == 0 || strcmp(cmd, "-u") == 0)) {
		fputs("Error: Invalid command entered!\n", stderr);
		return 1;
	}
	
	struct sockaddr_in local, remote;
	socklen_t len=sizeof(local), rlen=sizeof(remote);
	int sk;
	char sendbuf[BUFMAX], recvbuf[BUFMAX];

	if ((sk = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		close(sk);
		return 1;
	}

	local.sin_family = AF_INET;
	local.sin_port = htons(CLIENT_PORT);
	local.sin_addr.s_addr = INADDR_ANY;

	if (bind(sk, (struct sockaddr *)&local, len) < 0) {
		perror("bind error");
		close(sk);
		return 1;
	}

	remote.sin_family = AF_INET;
	remote.sin_port = htons(SERVER_PORT);
	if (get_server_addr(server, &remote.sin_addr) == -1) {
		fputs("Error: server is unreachable!\n", stderr);
		close(sk);
		return 1;
	}

	struct packet_t pkt;
	if (construct_pkt(argc, argv, &pkt) < 0) {
		fputs("Error: unable to construct packet!\n", stderr);
		close(sk);
		return 1;
	}
	memcpy(sendbuf, &pkt, sizeof(pkt));

	if (connect(sk, (struct sockaddr *)&remote, rlen) < 0) {
		perror("connect error");
		close(sk);
		return 1;
	}

	if (send(sk, sendbuf, sizeof(struct packet_t), 0) < 0) {
		perror("send error");
		close(sk);
		return 1;
	}

	if (recv(sk, recvbuf, sizeof(struct packet_t), 0) < 0) {
		perror("recv error");
		close(sk);
		return 1;
	}
	memcpy(&pkt, recvbuf, sizeof(struct packet_t));

	int ptype = ntohl(pkt.header.ptype);
	if (ptype == SUMSQRT || ptype == MESSAGE || ptype == ERROR) {
		printf("%s\n", pkt.body.message);
	} else if (ptype == UPDATE) {
		size_t count = ntohl(pkt.body.update.count);
		for (size_t i = 0; i < count; i++) {
			int * ip = (int *)&pkt.body.update.values[i];
			*ip = ntohl(*ip);
			printf("%.1f ", pkt.body.update.values[i]);
		} printf("\n");
	} else {
		fputs("Error: Server returned invalid packet!\n", stderr);
	}

	close(sk);
	return 0;
}
