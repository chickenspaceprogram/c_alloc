// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

// SPDX-License-Identifier: MPL-2.0

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <c_alloc/alloc.h>

void *c_alloc_allocator_alloc(size_t memsize, struct c_alloc_allocator *alloc)
{
	if (alloc == NULL)
		return malloc(memsize);

	return alloc->alloc(memsize, alloc->ctx);
}

void c_alloc_allocator_free(void *mem, size_t memsize, struct c_alloc_allocator *alloc)
{
	if (alloc == NULL) {
		free(mem);
		return;
	}
	
	if (alloc->free != NULL)
		alloc->free(mem, memsize, alloc->ctx);
}


void *c_alloc_allocator_realloc(void *mem, size_t newsize, size_t oldsize, struct c_alloc_allocator *alloc)
{
	if (alloc == NULL)
		return realloc(mem, newsize);
	
	if (alloc->realloc == NULL) {
		void *newbuf = alloc->alloc(newsize, alloc->ctx);
		if (newbuf == NULL) {
			return NULL;
		}
		memcpy(newbuf, mem, oldsize);
		c_alloc_allocator_free(mem, oldsize, alloc);
		return newbuf;
	}
	return alloc->realloc(mem, newsize, oldsize, alloc->ctx);
}

int c_alloc_allocator_try_realloc(void **mem, size_t newsize, size_t oldsize, struct c_alloc_allocator *alloc)
{
	void *newptr = c_alloc_allocator_realloc(*mem, newsize, oldsize, alloc);
	if (newptr == NULL)
		return -1;
	*mem = newptr;
	return 0;
}

static bool check_mult_overflow(size_t n1, size_t n2)
{
	// should in theory work
	size_t div1 = SIZE_MAX / n1;
	if (div1 < n2) {
		return true;
	}
	return false;
}

void *c_alloc_allocator_reallocarray(void *mem, size_t new_nel, size_t old_nel, size_t elem_size, struct c_alloc_allocator *alloc)
{
	if (check_mult_overflow(new_nel, elem_size)) {
		return NULL;
	}
	return c_alloc_allocator_realloc(mem, new_nel * elem_size, old_nel * elem_size, alloc);
}

int c_alloc_allocator_try_reallocarray(void **mem, size_t new_nel, size_t old_nel, size_t elem_size, struct c_alloc_allocator *alloc)
{
	void *newptr = c_alloc_allocator_reallocarray(*mem, new_nel, old_nel, elem_size, alloc);
	if (newptr == NULL)
		return -1;
	*mem = newptr;
	return 0;
}

void *c_alloc_allocator_allocarray(size_t nel, size_t elemsize, struct c_alloc_allocator *alloc)
{
	return c_alloc_allocator_alloc(nel * elemsize, alloc);
}

void c_alloc_allocator_freearray(void *mem, size_t nel, size_t elemsize, struct c_alloc_allocator *alloc)
{
	c_alloc_allocator_free(mem, nel * elemsize, alloc);
}

void *c_alloc_allocator_reallocf(void *mem, size_t newsize, size_t oldsize, struct c_alloc_allocator *alloc)
{
	void *result = c_alloc_allocator_realloc(mem, newsize, oldsize, alloc);
	if (result == NULL) {
		c_alloc_allocator_free(mem, oldsize, alloc);
		return NULL;
	}
	return result;
}
