#ifndef __MEMORY_HEADER_INCLUDED__
#define __MEMORY_HEADER_INCLUDED__

#include <stdio.h>
#include <stdlib.h>

/* for each of the 8 accessible regions in Z80 memory space,
there is a ordered list of MemoryBlock structures.
Each MemoryBlock structure has a priority and a pointer to the
data */

/* the order of each list changes when a MemoryBlock is added
or removed from the associated list */

typedef struct _MemoryBlock
{
	/* the priority of this block */
	int Priority;
	/* the pointer to the data */
	unsigned char *pPtr;

	/* the links to the next block in chain */
	struct _MemoryBlock *pPrev;
	struct _MemoryBlock *pNext;
} MemoryBlock;

/* add a memory block to the region specified */
void	Memory_AddRAMReadMemoryBlock(int RegionIndex, MemoryBlock *);
void	Memory_AddRAMWriteMemoryBlock(int RegionIndex, MemoryBlock *);
/* remove a memory block from the region specified */
void	Memory_RemoveRAMReadMemoryBlock(int RegionIndex, MemoryBlock *);
void	Memory_RemoveRAMWriteMemoryBlock(int RegionIndex, MemoryBlock *);

/* add a memory block to the region specified */
void	Memory_AddROMReadMemoryBlock(int RegionIndex, MemoryBlock *);
void	Memory_AddROMWriteMemoryBlock(int RegionIndex, MemoryBlock *);
/* remove a memory block from the region specified */
void	Memory_RemoveROMReadMemoryBlock(int RegionIndex, MemoryBlock *);
void	Memory_RemoveROMWriteMemoryBlock(int RegionIndex, MemoryBlock *);

/* update the memory configuration */
void	Memory_Rethink(void);
void	Memory_Finish(void);
void	Memory_Init(void);


unsigned long Memory_GetReadFlags(void);

void	Memory_SetReadFlags(unsigned long Flags);

unsigned long	Memory_GetWriteFlags(void);

void	Memory_SetWriteFlags(unsigned long Flags);


#endif
