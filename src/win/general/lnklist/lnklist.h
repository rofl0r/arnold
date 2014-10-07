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
#ifndef __DOUBLE_LINK_LIST__
#define __DOUBLE_LINK_LIST__

#define LINKLIST_TRAVERSE_STOP 1			/* Stop traversal of list */
#define LINKLIST_TRAVERSE_CONTINUE 0	/* Continue traversal of list */

/* Double Linked List stuff */

typedef struct 
{
	void	*pNodeData;							/* Pointer to node specific data */
	struct LIST_NODE	*pPreviousNode;	/* Pointer to previous node in list, NULL if first in list */
	struct LIST_NODE	*pNextNode;			/* Pointer to next node in list, NULL if last in list */
} LIST_NODE;


typedef struct	
{
	int			ItemCount;					/* Number of nodes in list */
	LIST_NODE	*pFirstNode;				/* pointer to first node in list */
	LIST_NODE	*pLastNode;					/* pointer to last node in list */
} LIST_HEADER;

/* List handling functions */

void	LinkList_InitialiseList(LIST_HEADER **);
void	LinkList_DeleteList(LIST_HEADER **,void (*)(void *));
void	LinkList_EmptyList(LIST_HEADER *,void (*)(void *));
void	LinkList_AddItemToListEnd(LIST_HEADER *,void *);
void	LinkList_AddItemToListStart(LIST_HEADER *,void *);
void	LinkList_InsertBeforeItem(LIST_HEADER *,LIST_NODE *,void *);
void	LinkList_InsertAfterItem(LIST_HEADER *,LIST_NODE *,void *);
void	LinkList_TraverseListForwards(LIST_HEADER *,void *,int (*)(void *,void *));
void	LinkList_TraverseListBackwards(LIST_HEADER *,void *,int (*)(void *,void *));
void	LinkList_DeleteItem(LIST_HEADER *, LIST_NODE *, void (*)(void *));
LIST_NODE *LinkList_SearchListForwards(LIST_HEADER *, void *, int (*)(void *,void *));
LIST_NODE *LinkList_SearchListBackwards(LIST_HEADER *, void *, int (*)(void *,void *));
void	LinkList_BubbleSortList(LIST_HEADER *pListHeader,int (*pCallBack)(LIST_NODE *,LIST_NODE *));

#endif
