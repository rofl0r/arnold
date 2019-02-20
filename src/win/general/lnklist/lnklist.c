/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <stdio.h>
#include <stdlib.h>

#include "lnklist.h"

/* Initialise a double linked list */
void LinkList_InitialiseList(LIST_HEADER **ppListHeader)
{
LIST_HEADER *pListHeader;

	pListHeader = (LIST_HEADER *)malloc(sizeof(LIST_HEADER));

	if (pListHeader!=NULL)
	{
		pListHeader->pFirstNode = NULL;
		pListHeader->pLastNode = NULL;
		pListHeader->ItemCount = 0;
	}
	
	*ppListHeader = pListHeader;
}

/* Delete all nodes in list, delete node data using user supplied callback,
and delete list header */

void	LinkList_DeleteList(LIST_HEADER **ppListHeader,void (*pDeleteCallback)(void *))
{
	LIST_HEADER *pListHeader;

	pListHeader = *ppListHeader;

	if (pListHeader!=NULL)
	{	
		LinkList_EmptyList(pListHeader,pDeleteCallback);
	
		free(pListHeader);
		*ppListHeader=NULL;
	}
}

/* Delete all nodes in list, delete node data using user supplied callback,
do not delete list header - list can be reused */

void	LinkList_EmptyList(LIST_HEADER *pListHeader,void (*pDeleteCallback)(void *))
{
LIST_NODE *pNodeToDelete;
LIST_NODE *pNextNode;

	if (pListHeader==NULL)
		return;
	
	pNodeToDelete = pListHeader->pFirstNode;
	
	while (pNodeToDelete!=NULL)
	{	
		pNextNode = pNodeToDelete->pNextNode;
		
		LinkList_DeleteItem(pListHeader,pNodeToDelete,pDeleteCallback);

		pNodeToDelete = pNextNode;
	}

	pListHeader->ItemCount=0;
}


/* Insert a item, with user data, after the item pointed to */

void	LinkList_InsertAfterItem(LIST_HEADER *pListHeader,LIST_NODE *pPreviousNode,void *pNodeData)
{
LIST_NODE	*pListNode;

	if (pPreviousNode == pListHeader->pLastNode)
	{
		LinkList_AddItemToListEnd(pListHeader,pNodeData);
	}
	
	pListNode = (LIST_NODE *)malloc(sizeof(LIST_NODE));
	pListNode->pNodeData = pNodeData;
	
	pListNode->pPreviousNode = pPreviousNode;
	pListNode->pNextNode = pPreviousNode->pNextNode;
	
	pPreviousNode->pNextNode = pListNode;

	pListHeader->ItemCount++;
}

/* Insert a item, with user data, before the item pointed to */

void	LinkList_InsertBeforeItem(LIST_HEADER *pListHeader,LIST_NODE *pNextNode, void *pNodeData)
{
LIST_NODE *pListNode;

	if (pNextNode == pListHeader->pFirstNode)
	{
		LinkList_AddItemToListStart(pListHeader,pNodeData);
	}
	
	pListNode = (LIST_NODE *)malloc(sizeof(LIST_NODE));
	pListNode->pNodeData = pNodeData;
	
	pListNode->pNextNode = pNextNode;
	pListNode->pPreviousNode = pNextNode->pPreviousNode;
	
	pNextNode->pPreviousNode = pListNode;

	pListHeader->ItemCount++;
}		

/* Add a item, with user data, to the end of the list */

void	LinkList_AddItemToListEnd(LIST_HEADER *pListHeader,void *pNodeData)
{
LIST_NODE	*pListNode;

	/* Allocate memory for Node */
	pListNode = (LIST_NODE *)malloc(sizeof(LIST_NODE));

	if (pListNode == NULL)
		return;

	/* Setup pointers in Node */
	pListNode->pNodeData = pNodeData;

	/* Does a list already exist? */
	if (pListHeader->pLastNode != NULL)
	{
		pListNode->pPreviousNode = pListHeader->pLastNode;
		pListNode->pNextNode = NULL;
	
		/* Set old end of list to point to new end of list */
	
		pListHeader->pLastNode->pNextNode = pListNode;	

		/* Setup pointer for end of list in list header */
		pListHeader->pLastNode = pListNode;
	}
	else
	{
		/* Setup list to point to this node */
		
		pListHeader->pFirstNode = pListNode;
		pListHeader->pLastNode = pListNode;

		/* Setup previous and next node pointers */

		pListNode->pPreviousNode = NULL;
		pListNode->pNextNode = NULL;		
	}

	pListHeader->ItemCount++;
}

/* Add a item, with user data, to the start of the list */

void	LinkList_AddItemToListStart(LIST_HEADER *pListHeader, void *pNodeData)
{
LIST_NODE	*pListNode;

	/* Allocate memory for Node */
	pListNode = (LIST_NODE *)malloc(sizeof(LIST_NODE));

	if (pListNode==NULL)
		return;

	/* Setup pointers in Node */
	pListNode->pNodeData = pNodeData;

	/* Does a list already exist? */
	if (pListHeader->pLastNode != NULL)
	{
		pListNode->pPreviousNode = NULL;
		pListNode->pNextNode = pListHeader->pFirstNode;
		
		/* Set old end of list to point to new end of list */
	
		pListHeader->pFirstNode->pPreviousNode = pListNode;	

		/* Setup pointer for end of list in list header */

		pListHeader->pFirstNode = pListNode;
	}
	else
	{
		/* Setup list to point to this node */
		
		pListHeader->pFirstNode = pListNode;
		pListHeader->pLastNode = pListNode;

		/* Setup previous and next node pointers */

		pListNode->pPreviousNode = NULL;
		pListNode->pNextNode = NULL;		
	}

	pListHeader->ItemCount++;
}

/* Traverse list forwards, executing a user supplied callback for each
node, with possiblity to stop traversal at any time */

void	LinkList_TraverseListForwards(LIST_HEADER *pListHeader,void *pData,int (*pCallback)(void *,void *))
{
LIST_NODE *pListNode;
int Status;

	if (pListHeader->pFirstNode == NULL)
		return;

	if (pCallback == NULL)
		return;
		
	pListNode = pListHeader->pFirstNode;
		
	do
	{
		/* Execute callback to display item in list if necessary */
		
		Status = pCallback(pData,pListNode->pNodeData);
		
		if (Status == LINKLIST_TRAVERSE_STOP)
			return;

		/* Go to next node */
		pListNode = pListNode->pNextNode;
	}
	while (pListNode != NULL);
}

/* Traverse list backwards, executing a user supplied callback for
every node, with possibility to stop traversal */

void	LinkList_TraverseListBackwards(LIST_HEADER *pListHeader,void *pData,int (*pCallback)(void *,void *))
{
LIST_NODE *pListNode;
int Status;

	if (pListHeader->pLastNode == NULL)
		return;

	if (pCallback == NULL)
		return;
		
	pListNode = pListHeader->pLastNode;
		
	do
	{
		/* Execute callback to display item in list if necessary */
		
		Status = pCallback(pData,pListNode->pNodeData);
		
		if (Status == LINKLIST_TRAVERSE_STOP)
			return;
		
		/* Go to next node */
		pListNode = pListNode->pPreviousNode;
	}
	while (pListNode != NULL);
}

/* Delete any item from the list, and if user supplied callback is provided
this is called to free up any user supplied node data */

void	LinkList_DeleteItem(LIST_HEADER *pListHeader, LIST_NODE *pNode, void (*pDeleteItemCallback)(void *))
{
LIST_NODE *pPreviousNode;
LIST_NODE *pNextNode;
	
	if (pListHeader == NULL)
		return;
		
	if (pNode == NULL)
		return;

	if ((pListHeader->pFirstNode == NULL) && (pListHeader->pLastNode == NULL))
			return;
	
	pPreviousNode = pNode->pPreviousNode;
	pNextNode = pNode->pNextNode;

	if (pPreviousNode != NULL)
	{
		pPreviousNode->pNextNode = pNextNode;
	}
	else
	{
		/* Node was start of list */
		
		pListHeader->pFirstNode = pNextNode;
	}
	
	if (pNextNode != NULL)
	{
		pNextNode->pPreviousNode = pPreviousNode;
	}
	else
	{
		/* Node was end of list */
		
		pListHeader->pLastNode = pPreviousNode;
	}

	if (pDeleteItemCallback != NULL)
	{
		pDeleteItemCallback(pNode->pNodeData);
	}
	free(pNode);	

	pListHeader->ItemCount--;
}

/* Scan through list forwards, for each node execute a user 
supplied function. This function is given the input node data, and the
current node data. User can perform their own comparison or whatever,
if result is 0, then scan stops and node pointer is returned */

LIST_NODE	*LinkList_SearchListForwards(LIST_HEADER *pListHeader, void *pNodeData, int (*pCompareCallback)(void *,void *))
{
LIST_NODE *pListNode;

int result;

	if (pListHeader->pFirstNode == NULL)
		return NULL;

	if (pCompareCallback == NULL)
		return NULL;
		
	pListNode = pListHeader->pFirstNode;
		
	do
	{
		/* Execute callback to display item in list if necessary */
		
		result = pCompareCallback(pNodeData,pListNode->pNodeData);

		if (result==0)
			return pListNode;
		
		/* Go to next node */
		pListNode = pListNode->pNextNode;
	}
	while (pListNode != NULL);

	return NULL;
}

/* Same as SearchListForwards, but starting from the end of the list and
going to the front */

LIST_NODE	*LinkList_SearchListBackwards(LIST_HEADER *pListHeader, void *pNodeData, int (*pCompareCallback)(void *,void *))
{
LIST_NODE *pListNode;
int result;

	if (pListHeader->pLastNode == NULL)
		return NULL;

	if (pCompareCallback == NULL)
		return NULL;
		
	pListNode = pListHeader->pLastNode;
		
	do
	{
		/* Execute callback to display item in list if necessary */
		
		result = pCompareCallback(pNodeData,pListNode->pNodeData);

		if (result == 0)
			return pNodeData;
					
		/* Go to next node */
		pListNode = pListNode->pPreviousNode;
	}
	while (pListNode != NULL);
	
	return NULL;
}

/* Perform a bubble sort of the linked list. For each node to be compared,
a user supplied callback is executed. The following should be returned:

0 for both are equal, -1 for first should be before 2nd, +1 for second should
be before first */

void	LinkList_BubbleSortList(LIST_HEADER *pListHeader,int (*pCallBack)(LIST_NODE *,LIST_NODE *))
{
	return;
}

