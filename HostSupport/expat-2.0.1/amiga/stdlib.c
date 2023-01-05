// Copyright Contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
// https://github.com/AcademySoftwareFoundation/OpenFx

#include <stdlib.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/utility.h>

void * malloc (size_t len)
{
	uint32 size = sizeof(uint32) + len;

	uint32 *mem = AllocMem(size, MEMF_SHARED);
	if ( mem != 0 )  {
		*mem = size;
		++mem;
	}

	return mem;
}


void * realloc (void * mem, size_t len2)
{
	if ( mem == 0 )  {
		return malloc(len2);
	}

	if ( len2 == 0 )  {
		free(mem);
		return 0;
	}

	void * new_mem = malloc(len2);
	if ( new_mem == 0 )  {
		return 0;
	}

	uint32 mem_size = *(((uint32*)mem) - 1);
	CopyMem(mem, new_mem, mem_size);
	free(mem);

	return new_mem;
}


void free (void * mem)
{
	if ( mem != 0 )  {
		uint32 * size_ptr = ((uint32*)mem) - 1;
		FreeMem(size_ptr, *size_ptr);
	}
}


int memcmp (const void * a, const void * b, size_t len)
{
	size_t i;
	int diff;

	for ( i = 0; i < len; ++i )  {
		diff = *((uint8 *)a++) - *((uint8 *)b++);
		if ( diff )  {
			return diff;
		}
	}

	return 0;
}


void * memcpy (void * t, const void * a, size_t len)
{
	CopyMem((APTR)a, t, len);
	return t;
}


void * memmove (void * t1, const void * t2, size_t len)
{
	MoveMem((APTR)t2, t1, len);
	return t1;
}


void * memset (void * t, int c, size_t len)
{
	return SetMem(t, c, len);
}
