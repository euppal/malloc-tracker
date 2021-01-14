// malloc-tracker: tracker.c
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

#define _MALLOC_TRACKER_TRACK
#include "_tracker.h"
#include <stdio.h>

static malloc_trace_t trace;

#define trace_abort(fmt, ...) do { \
    fprintf(stderr, "malloc-trace abort:%d: " fmt, __LINE__, ## __VA_ARGS__); \
    exit(EXIT_FAILURE); \
} while (0)
#define trace_warning(fmt, ...) do { \
    fprintf(stderr, "malloc-trace warning:%d: " fmt, __LINE__, ## __VA_ARGS__); \
} while (0)

void* tmalloc(size_t n) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    void* block = malloc(n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    if (block == NULL) {
        trace_abort("Virtual memory exhausted\n");
    }
    if (trace.allocations != NULL) {
        trace_append(&trace,
                     (allocation_t){
                         .previous = NULL,
                         .pointer = block,
                         .length = n,
                         .state = ALLOCATION_STATE_ALLOCATED,
                         .start = start.tv_nsec,
                         .end = end.tv_nsec
                     });
        trace.footprint += n;
    }
    return block;
}

void* trealloc(void* ptr, size_t n) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    void* block = realloc(ptr, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    if (block == NULL) {
        trace_abort("Virtual memory exhausted\n");
    }
    if (trace.allocations != NULL) {
        if (ptr != NULL) {
            allocation_t* previous_allocation = trace_search(&trace, ptr);
            if (previous_allocation == NULL) {
                trace_abort("Cannot reallocated an object not previously allocated\n");
            }
            trace.footprint -= previous_allocation->length;
        }
        trace_append(&trace, (allocation_t){
            .previous = ptr,
            .pointer = block,
            .length = n,
            .state = ALLOCATION_STATE_REALLOCATED,
            .start = start.tv_nsec,
            .end = end.tv_nsec
        });
        trace.footprint += n;
    }
    return block;
}

void tfree(void* ptr) {
    allocation_t* allocation;
    if (trace.allocations != NULL) {
        switch (trace_ptr_was_freed(&trace, ptr, &allocation)) {
            case TRACE_FREE_ISSUE_NONE: {
                break;
            }
            case TRACE_FREE_ISSUE_FREED: {
                trace_warning("Pointer being freed was already freed (%p)\n", ptr);
                return;
            }
            case TRACE_FREE_ISSUE_NOTALLOC: {
                trace_abort("Pointer being freed was not allocated (%p)\n", ptr);
            }
        }
    }
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    free(ptr);
    clock_gettime(CLOCK_MONOTONIC, &end);
    if (trace.allocations != NULL) {
        const size_t length = allocation->length;
        trace.footprint -= length;
        trace_append(&trace, (allocation_t){
            .previous = ptr,
            .pointer = NULL,
            .length = length,
            .state = ALLOCATION_STATE_FREED,
            .start = start.tv_nsec,
            .end = end.tv_nsec
        });
    }
}

static void trace_append(malloc_trace_t* trace, allocation_t allocation) {
    if (trace->length + 1 > trace->capacity) {
        trace->capacity *= 2;
        trace->allocations = (allocation_t*)realloc(trace->allocations,
                                                    sizeof(allocation_t)
                                                    * trace->capacity);
        if (trace->allocations == NULL) {
            trace_abort("Unable to continue tracing as virtual memory is exhausted\n");
        }
    }
    trace->allocations[trace->length++] = allocation;
}

static allocation_t* trace_search(malloc_trace_t* trace, void* ptr) {
    allocation_t* allocation = trace->allocations;
    for (size_t i = 0; i < trace->length; i++) {
        if (allocation->pointer == ptr) {
            return allocation;
        }
        allocation++;
    }
    return NULL;
}

static trace_free_issue_t trace_ptr_was_freed(malloc_trace_t* trace, void* ptr,
                                              allocation_t** allocation_) {
    *allocation_ = NULL;
    allocation_t* allocation = trace->allocations + trace->length - 1;
    for (size_t i = 0; i < trace->length; i++) {
        if (allocation->previous == ptr
            && allocation->state == ALLOCATION_STATE_FREED) {
            return TRACE_FREE_ISSUE_FREED;
        } else if (allocation->pointer == ptr) {
            *allocation_ = allocation;
            return TRACE_FREE_ISSUE_NONE;
        }
        allocation--;
    }
    return TRACE_FREE_ISSUE_NOTALLOC;
}

void tinit() {
    trace.length = 0;
    trace.capacity = 4;
    trace.allocations = (allocation_t*)malloc(sizeof(allocation_t)
                                                     * trace.capacity);
    trace.footprint = 0;

    if (trace.allocations == NULL) {
        trace_abort("Unable to setup tracing as virtual memory is exhausted\n");
    }
}

void tdestroy() {
    free(trace.allocations);
    trace.allocations = NULL;
}


void tdump() {
    allocation_t* allocation = trace.allocations;
    const size_t old_length = trace.length;
    for (size_t i = 1; i <= old_length; i++) {
        fprintf(stderr, "%04zu: ", i);
        switch (allocation->state) {
            case ALLOCATION_STATE_ALLOCATED: {
                fprintf(stderr, "%zu bytes allocated (%p)",
                        allocation->length,
                        allocation->pointer);
                break;
            }
            case ALLOCATION_STATE_REALLOCATED: {
                fprintf(stderr, "Reallocated %p to %zu bytes (%p)",
                        allocation->previous,
                        allocation->length,
                        allocation->pointer);
                break;
            }
            case ALLOCATION_STATE_FREED: {
                fprintf(stderr, "%zu bytes freed (%p)",
                        allocation->length,
                        allocation->previous);
                break;
            }
        }
        fprintf(stderr, " in %ldμs\n", (allocation->end - allocation->start) / 1000);
        allocation++;
    }
}

inline size_t tusage() {
    return trace.footprint;
}
