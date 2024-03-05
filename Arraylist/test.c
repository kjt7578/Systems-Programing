#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "arraylist.h"

int main(int argc, char **argv)
{
    arraylist_t A;
    al_init(&A, 4);

    for (int i = 0; i < 20; i++) {
	al_push(&A, i);
    }

    printf("list length: %u\n", al_length(&A));


    int n;
    while (al_pop(&A, &n)) {
	printf("popped %d\n", n);
    }

    al_destroy(&A);

    return EXIT_SUCCESS;
}
