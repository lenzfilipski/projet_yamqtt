#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
void copy (char* a, char* b, size_t len) {
	// copy a to b
	// a and b must be the same size
	for (int i=0; i<len; i++)
		b[i] = a[i];
}
*/

void swap_int (int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void swap_char(char** a, char** b) {
	size_t len_a = strlen(*a);
	size_t len_b = strlen(*b);
	
	if (len_a != len_b) {
		printf("Strings must be the same lenght!\n");
		return;
	}

	char* temp = (char *) malloc(len_a);
	temp = *a;
	*a = *b;
	*b = temp;
}

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

