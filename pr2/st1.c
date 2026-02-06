#include <stdio.h>

int bss;
int data = 10;

int func(void) {
	return 10;
}

int main(void) {
	int i;
	printf("The stack top is near %p\n", &i);
	printf("bss: %p\n", &bss);
	printf("data: %p\n", &data);
	printf("text: %p\n", func);

	int arr[400];	
	for (int i = 0; i < 400; i++) {
		arr[i] = func();
	}
	int j;

	printf("(2) The stack top is near %p\n", &j);
	return 0;
}
