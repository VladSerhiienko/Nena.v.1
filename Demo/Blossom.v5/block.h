#ifndef __BLOCK_H__
#define __BLOCK_H__

#include <stdlib.h>

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

template <class Type> class Block
{
public:
	/* Constructor. Arguments are the block size and
	   (optionally) the pointer to the function which
	   will be called if allocation failed; the message
	   passed to this function is "Not enough memory!" */
	Block(int size, void (*err_function)(const char *) = NULL) { first = last = NULL; block_size = size; error_function = err_function; }

	/* Destructor. Deallocates all items added so far */
	~Block() { while (first) { block *next = first -> next; delete[] ((char*)first); first = next; } }

	/* Allocates 'num' consecutive items; returns pointer
	   to the first item. 'num' cannot be greater than the
	   block size since items must fit in one block */
	Type *New(int num = 1)
	{
		Type *t;

		if (!last || last->current + num > last->last)
		{
			if (last && last->next) last = last -> next;
			else
			{
				block *next = (block *) new char [sizeof(block) + (block_size-1)*sizeof(Type)];
				if (!next) { if (error_function) (*error_function)("Not enough memory!"); exit(1); }
				if (last) last -> next = next;
				else first = next;
				last = next;
				last -> current = & ( last -> data[0] );
				last -> last = last -> current + block_size;
				last -> next = NULL;
			}
		}

		t = last -> current;
		last -> current += num;
		return t;
	}

	/* Returns the first item (or NULL, if no items were added) */
	Type *ScanFirst()
	{
		for (scan_current_block=first; scan_current_block; scan_current_block = scan_current_block->next)
		{
			scan_current_data = & ( scan_current_block -> data[0] );
			if (scan_current_data < scan_current_block -> current) return scan_current_data ++;
		}
		return NULL;
	}

	/* Returns the next item (or NULL, if all items have been read)
	   Can be called only if previous ScanFirst() or ScanNext()
	   call returned not NULL. */
	Type *ScanNext()
	{
		while (scan_current_data >= scan_current_block -> current)
		{
			scan_current_block = scan_current_block -> next;
			if (!scan_current_block) return NULL;
			scan_current_data = & ( scan_current_block -> data[0] );
		}
		return scan_current_data ++;
	}

	/* Marks all elements as empty */
	void Reset()
	{
		block *b;
		if (!first) return;
		for (b=first; ; b=b->next)
		{
			b -> current = & ( b -> data[0] );
			if (b == last) break;
		}
		last = first;
	}

/***********************************************************************/

private:

	typedef struct block_st
	{
		Type					*current, *last;
		struct block_st			*next;
		Type					data[1];
	} block;

	int		block_size;
	block	*first;
	block	*last;

	block	*scan_current_block;
	Type	*scan_current_data;

	void	(*error_function)(const char *);
};

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

template <class Type> class DBlock
{
public:
	/* Constructor. Arguments are the block size and
	   (optionally) the pointer to the function which
	   will be called if allocation failed; the message
	   passed to this function is "Not enough memory!" */
	DBlock(int size, void (*err_function)(const char *) = NULL) { first = NULL; first_free = NULL; block_size = size; error_function = err_function; }

	/* Destructor. Deallocates all items added so far */
	~DBlock() { while (first) { block *next = first -> next; delete[] ((char*)first); first = next; } }

	/* Allocates one item */
	Type *New()
	{
		block_item *item;

		if (!first_free)
		{
			block *next = first;
			first = (block *) new char [sizeof(block) + (block_size-1)*sizeof(block_item)];
			if (!first) { if (error_function) (*error_function)("Not enough memory!"); exit(1); }
			first_free = & (first -> data[0] );
			for (item=first_free; item<first_free+block_size-1; item++)
				item -> next_free = item + 1;
			item -> next_free = NULL;
			first -> next = next;
		}

		item = first_free;
		first_free = item -> next_free;
		return (Type *) item;
	}

	/* Deletes an item allocated previously */
	void Delete(Type *t)
	{
		((block_item *) t) -> next_free = first_free;
		first_free = (block_item *) t;
	}

/***********************************************************************/

private:

	typedef union block_item_st
	{
		Type			t;
		block_item_st	*next_free;
	} block_item;

	typedef struct block_st
	{
		struct block_st			*next;
		block_item				data[1];
	} block;

	int			block_size;
	block		*first;
	block_item	*first_free;

	void	(*error_function)(const char *);
};


#endif

