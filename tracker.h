// malloc-tracker: tracker.h
// Copyright (C) 2021 Ethan Uppal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef _MALLOC_TRACKER_H
#define _MALLOC_TRACKER_H

#define _POSIX_C_SOURCE 0x200809L
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    ALLOCATION_STATE_ALLOCATED,
    ALLOCATION_STATE_REALLOCATED,
    ALLOCATION_STATE_FREED
} allocation_state_t;

typedef struct {
    void* previous;
    void* pointer;
    size_t length;
    allocation_state_t state;
    long start;
    long end;
} allocation_t;

typedef struct {
    size_t length;
    size_t capacity;
    allocation_t* allocations;
    size_t footprint;
} malloc_trace_t;

#ifdef _MALLOC_TRACKER_TRACK
void* tmalloc(size_t n);
void* trealloc(void* ptr, size_t n);
void tfree(void* ptr);
#else
#define tmalloc malloc
#define trealloc realloc
#define tfree free
#endif

void tinit(void);
void tdestroy(void);
void tdump(void);
size_t tusage(void);

#endif
