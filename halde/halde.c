#include "halde.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/// Magic value for occupied memory chunks.
#define MAGIC ((void*)0xbaadf00d)

/// Size of the heap (in bytes).
#define SIZE (1024*1024*1)

/// Memory-chunk structure.
struct mblock {
	struct mblock *next;
	size_t size;
	char memory[];
};

/// Heap-memory area.
static char memory[SIZE];

/// Pointer to the first element of the free-memory list.
static struct mblock *head;

/// Helper function to visualise the current state of the free-memory list.
void printList(void) {
	struct mblock *lauf = head;

	// Empty list
	if (head == NULL) {
		char empty[] = "(empty)\n";
		write(STDERR_FILENO, empty, sizeof(empty));
		return;
	}

	// Print each element in the list
	const char fmt_init[] = "(off: %7zu, size:: %7zu)";
	const char fmt_next[] = " --> (off: %7zu, size:: %7zu)";
	const char * fmt = fmt_init;
	char buffer[sizeof(fmt_next) + 2 * 7];

	while (lauf) {
		size_t n = snprintf(buffer, sizeof(buffer), fmt
			, (uintptr_t) lauf - (uintptr_t)memory, lauf->size);
		if (n) {
			write(STDERR_FILENO, buffer, n);
		}

		lauf = lauf->next;
		fmt = fmt_next;
	}
	write(STDERR_FILENO, "\n", 1);
}

void *malloc (size_t size) {
	// TODO: implement me!
	if (size == 0) return NULL;

	// Empty list
	if (head == NULL) 
	{
		head = (struct mblock*) memory; // pointing to the beginning of memory
		head->size = SIZE - sizeof(struct mblock); // size of the whole memory
		head->next = NULL;
	}

	struct mblock* prev = NULL;
	struct mblock* curr = head;

	while(curr != NULL)
	{
		if(curr->size >= sizeof(struct mblock) + size) // enough size for at least one element
		{
			if (curr->size >= 2*sizeof(struct mblock) + size + 1) // enough size for at least two elements (split in that case)
			{
				struct mblock* new_node = (struct mblock*)((char*)curr + size + sizeof(struct mblock));
				new_node->size = curr->size - size - sizeof(struct mblock);
				new_node->next = curr->next;

				curr->size = size;

				if(prev == NULL)
				{
					head = new_node;
				}
				else
				{
					prev->next = new_node;
				}
			}
			else // enough size for exactly one element (no split)
			{
				if(prev == NULL)
				{
					head = curr->next;
				}
				else
				{
					prev->next = curr->next;
				}
			}

			curr->next = MAGIC;
			return curr->memory;
		}

		prev = curr;
		curr = curr->next;
	}

	errno = ENOMEM;
	return NULL; // no free memory found
}

void free (void *ptr) {
	// TODO: implement me!
	if (ptr == NULL) return;

	struct mblock* node = (struct mblock*) ((char*)ptr - sizeof(struct mblock));

	if (node->next != MAGIC)
	{
		abort();
	}

	node->next = head;
	head = node;
}

void *realloc (void *ptr, size_t size) {
	// TODO: implement me!
	if (ptr == NULL) {
        return malloc(size);
    }

	if (size == 0)
	{
		free(ptr);
		return NULL;
	}

	struct mblock* node = (struct mblock*) ((char*)ptr - sizeof(struct mblock));
	if (node->next != (struct mblock*)MAGIC)
	{
		abort();
	}

	void* new_ptr = malloc(size);
	if (new_ptr == NULL) 
	{
		errno = ENOMEM;
		return NULL;
	}

	size_t copy_size = node->size;
	if (node->size > size)
	{
		copy_size = size;
	}

	memcpy(new_ptr, ptr, copy_size);
	free(ptr);

	return new_ptr;
}

void *calloc (size_t nmemb, size_t size) {
	// TODO: implement me!
	size_t alloc_size = nmemb * size;
	
	void* ptr = malloc(alloc_size);
	if(ptr == NULL) return NULL;

	memset(ptr, 0, alloc_size);

	return ptr;
}
