#ifndef __FILE_LIST_HEADER_INCLUDED__
#define __FILE_LIST_HEADER_INCLUDED__

#include "../cpc/cpcglob.h"

typedef struct
{
	/* name */
	const char *name;
	unsigned long nSize;
	unsigned long nAttributes;
} file_list_item;

typedef struct _file_list_linked_item
{
	/* points to next item in list */
	struct _file_list_linked_item *next;
	/* this item */
	file_list_item item;
} file_list_linked_item;

typedef struct
{
	/* the first item */
	file_list_linked_item *first;
} file_list;

void file_list_init(file_list *);
file_list_linked_item *file_list_allocate_item(const char *name, unsigned long nSize);
void file_list_add_item(file_list *, file_list_linked_item *);
void file_list_delete(file_list *list);


#endif