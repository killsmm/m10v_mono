/**
 * @file		user_utility.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

// optimized O1 only
//#pragma GCC optimize ("O1")

#include "user.h"
#include "linked_list.h"
/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/
// We are enforcing a minimum allocation size of 32B.
#define MIN_ALLOC_SZ ALLOC_HEADER_SZ + 32

/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/
/**
* Simple macro for making sure memory addresses are aligned
* to the nearest power of two
*/
#ifndef align_up
#define align_up(num, align) \
	(((num) + ((align) - 1)) & ~((align) - 1))
#endif

/**
* We vend a memory address to the user.  This lets us translate back and forth
* between the vended pointer and the container we use for managing the data.
*/
#define ALLOC_HEADER_SZ offsetof(alloc_node_t, block)

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/
/*
* This is the container for our free-list.
* Node the usage of the linked list here: the library uses offsetof
* and container_of to manage the list and get back to the parent struct.
*/
typedef struct {
	ll_t node;
	size_t size;
	char * block;
} alloc_node_t;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/
// This macro simply declares and initializes our linked list
static LIST_INIT(free_list_ch0);
static LIST_INIT(free_list_ch1);

static unsigned int g_free_list_spin_lock	__attribute__((section(".LOCK_SECTION"), aligned(64)));
/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/
static void defrag_free_list(char ch);

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/
void User_Utility_MM_Init(void)
{
	Dd_ARM_Critical_Section_Start(g_free_list_spin_lock); // for safety
	free_list_ch0.prev = &free_list_ch0;
	free_list_ch0.next = &free_list_ch0;
	free_list_ch1.prev = &free_list_ch1;
	free_list_ch1.next = &free_list_ch1;
	Dd_ARM_Critical_Section_End(g_free_list_spin_lock); // for safety
}

void* User_Utility_MM_alloc(size_t size, char ch)
{
	void * ptr = NULL;
	alloc_node_t *blk = NULL;

	Dd_ARM_Critical_Section_Start(g_free_list_spin_lock); // for safety

	if(size > 0) {
		//Align the pointer
		size = align_up(size, sizeof(void *));

		if (ch == 0) {
			// try to find a big enough block to alloc
			list_for_each_entry(blk, &free_list_ch0, node) {
				if(blk->size >= size) {
					ptr = &blk->block;
					break;
				}
			}
		} else {
			// try to find a big enough block to alloc
			list_for_each_entry(blk, &free_list_ch1, node) {
				if(blk->size >= size) {
					ptr = &blk->block;
					break;
				}
			}
		}


		// we found something
		if(ptr) {
			// Can we split the block?
			if((blk->size - size) >= MIN_ALLOC_SZ) {
				alloc_node_t *new_blk;
				new_blk = (alloc_node_t *)((unsigned int)(&blk->block) + size);
				new_blk->size = blk->size - size - ALLOC_HEADER_SZ;
				blk->size = size;
				list_add_(&new_blk->node, &blk->node, blk->node.next);
				//UPRINTF("New sub block:%x, size:%x\n", (unsigned int)&new_blk->block, new_blk->size);
			}
			//UPRINTF("alloc sub block:%x, size:%x\n", (unsigned int)&blk->block, blk->size);
			list_del(&blk->node);
		}

	}

	Dd_ARM_Critical_Section_End(g_free_list_spin_lock); // for safety

	return ptr;
}

void User_Utility_MM_free(void * ptr, char ch)
{
	alloc_node_t *blk, *free_blk;

	Dd_ARM_Critical_Section_Start(g_free_list_spin_lock); // for safety
	//Don't free a NULL pointer..
	if(ptr)	{

		// we take the pointer and use container_of to get the corresponding alloc block
		blk = container_of(ptr, alloc_node_t, block);
		//UPRINTF("free sub block:%x, size:%x\n", (unsigned int)&blk->block, blk->size);
		if (ch == 0) {
			//Let's put it back in the proper spot
			list_for_each_entry(free_blk, &free_list_ch0, node)
			{
				if(free_blk > blk) {
					list_add_(&blk->node, free_blk->node.prev, &free_blk->node);
					goto blockadded;
				}
			}
			list_add_tail(&blk->node, &free_list_ch0);
		} else {
			//Let's put it back in the proper spot
			list_for_each_entry(free_blk, &free_list_ch1, node)
			{
				if(free_blk > blk) {
					list_add_(&blk->node, free_blk->node.prev, &free_blk->node);
					goto blockadded;
				}
			}
			list_add_tail(&blk->node, &free_list_ch1);
		}


blockadded:
		// Let's see if we can combine any memory
		defrag_free_list(ch);
	}

	Dd_ARM_Critical_Section_End(g_free_list_spin_lock); // for safety
}

void User_Utility_MM_addblock(void *addr, size_t size, char ch)
{
	alloc_node_t *blk;

	Dd_ARM_Critical_Section_Start(g_free_list_spin_lock); // for safety

	// let's align the start address of our block to the next pointer aligned number
	blk = (void *) align_up((unsigned int)addr, sizeof(void*));

	// calculate actual size - remove our alignment and our header space from the availability
	blk->size = (unsigned int) addr + size - (unsigned int) blk - ALLOC_HEADER_SZ;

	UPRINTF("New block:%x, size:%x\n", (unsigned int)&blk->block, blk->size);
	//and now our giant block of memory is added to the list!
	ch == 0 ? list_add(&blk->node, &free_list_ch0) : list_add(&blk->node, &free_list_ch1);

	Dd_ARM_Critical_Section_End(g_free_list_spin_lock); // for safety
}


/*==============================================================================*/
/* Local Function																*/
/*==============================================================================*/
/**
* When we free, we can take our node and check to see if any memory blocks
* can be combined into larger blocks.  This will help us fight against
* memory fragmentation in a simple way.
*/
void defrag_free_list(char ch)
{
	alloc_node_t *b, *lb = NULL, *t;

	if (ch == 0) {
		list_for_each_entry_safe(b, t, &free_list_ch0, node)
		{
			if(lb) {
				if((((unsigned int)&lb->block) + lb->size) == (unsigned int)b) {
					lb->size += ALLOC_HEADER_SZ + b->size;
					list_del(&b->node);
					//UPRINTF("combine block:%x, size:%x\n", (unsigned int)&lb->block, lb->size);
					continue;
				}
			}
			lb = b;
		}
	} else {
		list_for_each_entry_safe(b, t, &free_list_ch1, node)
		{
			if(lb) {
				if((((unsigned int)&lb->block) + lb->size) == (unsigned int)b) {
					lb->size += ALLOC_HEADER_SZ + b->size;
					list_del(&b->node);
					//UPRINTF("combine block:%x, size:%x\n", (unsigned int)&lb->block, lb->size);
					continue;
				}
			}
			lb = b;
		}
	}

}
