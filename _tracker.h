// malloc-tracker: _tracker.h
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

#include "tracker.h"
#include <stdlib.h>
#include <time.h>

typedef enum {
    TRACE_FREE_ISSUE_NONE,
    TRACE_FREE_ISSUE_FREED,
    TRACE_FREE_ISSUE_NOTALLOC
} trace_free_issue_t;

static void trace_append(malloc_trace_t* trace, allocation_t allocation);
static allocation_t* trace_search(malloc_trace_t* trace, void* ptr);
static trace_free_issue_t trace_ptr_was_freed(malloc_trace_t* trace, void* ptr,
                                              allocation_t** allocation_);
