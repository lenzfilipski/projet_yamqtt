#include <stdio.h>
#include <string.h>

#include "swap.h"

int main () {
	char* a = "42";
	char* b = "hello world";
	char* c = "salut monde";

	int d = 12, e = 42;
	printf("a: %s, b: %s, c: %s, d: %d, e: %d\n", a, b, c, d, e);
	printf("a: %p, b: %p, c: %p, d: %p, e: %p\n", &a, &b, &c, &d, &e);

	swap_char(&a, &b);
	swap_char(&b, &c);

	swap_int(&d, &e);

	printf("\n");

	printf("a: %s, b: %s, c: %s, d: %d, e: %d\n", a, b, c, d, e);

	return 0;
}

