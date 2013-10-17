#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>

int main()
{
	const size_t size  = 100000000;
	char *mem = malloc(size);
	mem[0] = 0;
	mem[size-1] = 0;
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);

	printf("%d\n", (int) rusage.ru_maxrss);
}
