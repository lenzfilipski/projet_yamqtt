#include <stdio.h>
#include <string.h>

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

	char* temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

