#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "halde.h"

int main(int argc, char *argv[]) {

	printList();

    char *m1 = malloc(200*1024);
    printList();

    char *m2 = malloc(200*1024);
    printList();

    char *m3 = malloc(200*1024);
    printList();

    char *m4 = malloc(200*1024);
    printList();

    free(m1);
    printList();

    free(m2);
    printList();

    free(m3);
    printList();

    free(m4);
    printList();

    m1 = malloc(100*1024);
    printList();

    m2 = malloc(100*1024);
    printList();

    m3 = malloc(100*1024);
    printList();

    m4 = malloc(100*1024);
    printList();

    free(m1);
    printList();

    free(m2);
    printList();

    free(m3);
    printList();

    free(m4);
    printList();

    // Randfall
    m1 = realloc(NULL, 1000*1024);
    printList();

    exit(EXIT_SUCCESS);

	// printList();

	// char *m1 = malloc(200*1024);
	// printList();

	// char *m2 = malloc(400);
	// printList();

	// char *m3 = malloc(800);
	// printList();
	
	// char *m4 = malloc(1600);
	// printList();

	// int *c1 = calloc(100, sizeof(int));
    // printList();

    // char *c2 = calloc(500, 1);
    // printList();

	// m1 = realloc(m1, 300*1024);
    // printList();

    // c2 = realloc(c2, 1000);
    // printList();

	// int *c3 = calloc(200, sizeof(int));
    // printList();

	// m1 = realloc(m1, 100*1024);
	// printList();

	// free(m1);
	// printList();

	// free(m2);
	// printList();

	// free(m3);
	// printList();

	// free(m4);
	// printList();

	// free(c1);
    // printList();

	// free(c2);
    // printList();

    // free(c3);
    // printList();

	// char *m5 = malloc(6745);
	// printList();

	// char *m6 = malloc(636);
	// printList();

	// char *m7 = malloc(0);
	// printList();
	
	// char *m8 = malloc(82772);
	// printList();

	// free(m5);
	// printList();

	// free(m6);
	// printList();

	// free(m7);
	// printList();

	// free(m8);
	// printList();

	exit(EXIT_SUCCESS);
}
