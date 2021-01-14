// malloc-tracker: main.c: A testing program for malloc-tracker.
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
#include "tracker.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct _list_t {
    int value;
    struct _list_t* next;
} list_t;

void print_free(list_t* list) {
    printf("node: %d\n", list->value);
    if (list->next != NULL) {
        print_free(list->next);
    }
    tfree(list);
}

int main(int argc, char const *argv[]) {
    tinit();

    list_t* head = (list_t*)tmalloc(sizeof(list_t));
    head->value = 1;
    list_t* list = head;
    for (size_t i = 0; i < 10; i++) {
        list->next = (list_t*)tmalloc(sizeof(list_t));
        list = list->next;
        list->value = rand();
    }
    list->next = NULL;

    printf("Before we free, the program has allocated %zu byte(s)\n", tusage());

    print_free(head);

    tdump();

    tdestroy();
    return 0;
}
