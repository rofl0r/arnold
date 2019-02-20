#include "file_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

file_list_linked_item *file_list_allocate_item(const char *sName, unsigned long nSize)
{
	int nStringLenBytes = strlen(sName)+1;

	file_list_linked_item *item = (file_list_linked_item *)malloc(sizeof(file_list_linked_item)+nStringLenBytes);

	if (item!=NULL)
	{
		item->next = NULL;
		item->item.name = (const char *)((unsigned long)item + sizeof(file_list_linked_item));
		item->item.nSize = nSize;
		item->item.nAttributes = 0;

		strcpy((char *)item->item.name, sName);
	}

	return item;
}

void file_list_init(file_list *list)
{
	list->first = NULL;
}

void file_list_add_item(file_list *list, file_list_linked_item *item)
{
	item->next = list->first;
	list->first = item;
}

void file_list_delete(file_list *list)
{
	file_list_linked_item *current, *next;

	current = list->first;

	while (current!=NULL)
	{
		next = current->next;
		
		free(current);

		current = next;
	}
}

