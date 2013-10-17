#include <stdarg.h>
int fc(int a, int b, int c, int d, int e) __attribute__((noinline));
int fca(int a, ...) __attribute__((noinline));

int fc(int a, int b, int c, int d, int e)
{
	return a + b + c + d + e;
}

int fca(int a, ...)
{
	va_list ap;
	va_start(ap, a);
	int b = va_arg(ap, int);
	int c = va_arg(ap, int);
	int d = va_arg(ap, int);
	int e = va_arg(ap, int);
	va_end(ap);
	return a + b + c + d + e;
}

int main()
{
	int sum = 0;
	for (int i = 0; i < 90000000; i++)
		sum += fca(i, i, i, i, i);
	return sum;
}
