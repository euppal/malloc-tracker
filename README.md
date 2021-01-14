# Malloc Tracker

Memory leaks are one of the most common programming errors and one of the easiest to create. Malloc Tracker attempts to provide a simple in-language interface to view the memory usage of your program provided you replace `malloc`, `realloc` and `free` with the variants that have a `t` prefix.

## Overview

To use this, simply add the following lines to the top of your program:

```c
#define _MALLOC_TRACKER_TRACK
#include "tracker.h"
```

You can just as easily undefine `_MALLOC_TRACKER_TRACK` to disable tracking functionality: `tmalloc`, `trealloc` and `tfree` are simply expanded to their standard library variants.

## Usage

To view the current memory footprint of a program, use `tusage`. To log the allocations, reallocations and frees in a format meant to be easily digestive by parsers to the standard error use `tdump`.

To initialize tracing, call `tinit`. To end tracing, call `tdestroy`. Note that any use of the tracing functions after `tdestroy` is undefined behavior, although I attempted to mitigate this in my design. Therefore, the `t*` functions will simply perform the standard library variants. However, calling `tdump`/`tusage` can produce a crash or unexpected results.

This repository contains an example program `main.c` to showcase these features.

## Licensing

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.

## Docs

**tmalloc**

```c
void* tmalloc(size_t n);
```

The tracing variant to `void* malloc(size_t)`.

**trealloc**

```c
void* trealloc(void* ptr, size_t n);
```

The tracing variant to `void* realloc(void*, size_t)`.

**tfree**

```c
void tfree(void* ptr);
```

The tracing variant to `void free(void*)`.

**tinit**

```c
void tinit(void);
```

Allocates any memory and does the necessary setup for the initialization of the tracing facilities.

**tdestroy**

```c
void tdestroy(void);
```

Frees any memory and does the necessary teardown of the deinitialization of the tracing facilities.

**tdump**

```c
void tdump(void);
```

Writes the existing history of allocations to the standard output.

**tusage**

```c
size_t tusage(void);
```

Returns the total bytes currently allocated.
