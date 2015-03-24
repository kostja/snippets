#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

static const char *http_end = "\r\n\r\n";
static const char *http_end1 = "\n\n";

const char *http_200_ok = {
	"HTTP/1.1 200 OK\r\n"
	"Connection: keep-alive\r\n"
	"Content-Type: text/xml;charset=utf-8\r\n"
	"Content-Length: 0\r\n"
	"\r\n"
};

int
http_request(int fd)
{
	int recvpos = 0;
	char recvbuf[4096];
	ssize_t n;
	while (1) {
		n = read(fd, recvbuf, sizeof(recvbuf) - recvpos);
		if (n < 0)
			error("ERROR reading from socket");
		if (n == 0)
			return 1;
		recvpos += n;
		recvbuf[recvpos] = '\0';
		if (strstr(recvbuf, http_end1) || strstr(recvbuf, http_end)) {
			break;
		}
	}
	n = write(fd, http_200_ok, strlen(http_200_ok));
	if (n < 0)
		error("ERROR writing to socket");
	return 0;
}

static double prev_time;
static int total_requests = 0;

void
http(int fd)
{
	int YARDSTIICK = 4000;
	int eof = 0;
	while (! eof) {
		if (++total_requests % YARDSTIICK == 0) {
			double cur_time = time(0);
			printf("rps: %d\n",
			       (int) (YARDSTIICK/(cur_time - prev_time)));
			prev_time = cur_time;
		}
		eof = http_request(fd);
	}
	close(fd);
}

void start()
{
	socklen_t len;
	struct sockaddr_in addr;

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);

	int yes = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &yes,
		       sizeof(int)) < 0)
		error("setsockopt");


	if (bind(listen_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		error("ERROR on binding");

	listen(listen_fd, 5);
	int addrlen;
	int fd = accept(listen_fd, NULL, NULL);
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *) &yes,
		       sizeof(int)) < 0)
		error("setsockopt2");
	http(fd);
	close(listen_fd);
}

int main()
{
	prev_time = time(0);
	start();
	printf("total requests: %d\n", total_requests);
}
