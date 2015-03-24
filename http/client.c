#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <arpa/inet.h>


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
http_request(int fd, const char *request, ssize_t len)
{
	int recvpos = 0;
	char recvbuf[4096];
	ssize_t n = write(fd, request, len);
	if (n != len)
		error("partial write");

	while (1) {
		n = read(fd, recvbuf, sizeof(recvbuf) - recvpos);
		if (n < 0)
			error("ERROR reading from socket");
		if (n == 0)
			return 1;
		recvpos += n;
		if (strstr(recvbuf, http_end) || strstr(recvbuf, http_end1))
			break;
	}
	if (n < 0)
		error("ERROR writing to socket");
	return 0;
}

void
http(int fd, char *log, size_t len)
{
	char *pos;
	while ((pos = strstr(log, http_end))) {
		http_request(fd, log, pos + strlen(http_end) - log);
		log = pos + strlen(http_end);
	}
	close(fd);
}

int http_connect()
{
	socklen_t len;
	struct sockaddr_in addr;

	int fd = socket(AF_INET, SOCK_STREAM, 0);

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		error("connect");

	int yes = 1;
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *) &yes,
		       sizeof(int)) < 0)
		error("setsockopt2");
	return fd;
}


char *
open_log(const char *filename, size_t *out_size)
{
	const int align = 4096;
	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		error("open");
	struct stat st;
	fstat(fd, &st);
	int size = (int) st.st_size;
	int size_aligned = (size + align) & ~(align - 1);
	char *log = mmap(NULL, size_aligned, PROT_READ | PROT_WRITE,
			 MAP_PRIVATE, fd, 0);
	if (log == MAP_FAILED)
		error("mmap");
	if (log[size] != 0)
		log[size] = 0;
	*out_size = size;
	return log;
}

int main()
{
	int socket = http_connect();
	ssize_t len;
	char *log = open_log("trx.http.log", &len);
	http(socket, log, len);
}
