#include <sys/time.h>
#include <stdio.h>

static inline double
mydifftime(struct timeval *t1, struct timeval *t2)
{
	return t2->tv_sec - t1->tv_sec - 1.0 +
		(1000000 + t2->tv_usec - t1->tv_usec)/1000000.0;
}

int main()

{
	struct timeval t1, t2;

	gettimeofday(&t1, 0);
	usleep(3000);
	gettimeofday(&t2, 0);
	printf("%f\n", mydifftime(&t1, &t2));
	return 0;
}
