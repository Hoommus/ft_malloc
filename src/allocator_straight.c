
#include "ft_malloc_private.h"
#include <assert.h>

static inline size_t		next_free_block(struct s_zone *zone, size_t index)
{
	size_t			i;
	struct s_block	*table;

	if (zone->block_table[index + 1].pointer == 0)
		return (index + 1);
	table = zone->block_table;
	i = index;
	while (i < zone->table_size)
	{
		if (table[i].is_free)
			return (i);
		i++;
	}
	zone->is_full = true;
	return (i);
}

static void		block_allocate(struct s_zone *zone, struct s_block *blk, size_t size)
{

}

/*
** I said `no homo', but it meant VERY HOMO
*/

void				*get_block_straight(struct s_zone *zone, size_t size)
{
	size_t			i;
	size_t			page_offset;
	size_t			desired_ptr;
	struct s_block	*table;
	struct s_block	*possible_block;

	possible_block = NULL;
	assert(size <= BLK_SMALL_MAX);
	assert((zone->type == BLK_TINY || zone->type == BLK_SMALL));
	if (size > g_storage->pagesize || zone->zone_size - zone->bytes_malloced < size)
		return (NULL);
	table = zone->block_table;
	i = 0;
	while (++i < zone->table_size - 1)
		if (table[i].pointer == 0 || table[i].is_free == true)
		{
			//printf("this pointer: %zx, last %zx\n", table[i].pointer, table[i - 1].pointer);
			write(1, "hello\n", 6);
			desired_ptr = table[i - 1].pointer + table[i - 1].size;
			page_offset = table[i].pointer & PAGE_ADDRESS_MASK;
			if (table[i + 1].pointer && table[i + 1].pointer < desired_ptr)
				continue ;
			if (page_offset + size >= g_storage->pagesize && !possible_block)
			{
				possible_block = table + i;
				continue ;
			}
			table[i].is_free = false;
			table[i].pointer = table[i - 1].pointer + size;
			table[i].size = size;
			zone->first_free_block_index = next_free_block(zone, i);
			g_storage->total_allocated += size;
			ft_putnbr((int)table[i].pointer);
			ft_putchar('\n');
			return ((void *)(table[i].pointer));
		}
	return (NULL);
}