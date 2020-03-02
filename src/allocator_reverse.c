/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator_reverse.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <vtarasiu@student.unit.ua>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 16:54:58 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/02/08 12:18:00 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <assert.h>

size_t							next_free_block(struct s_zone *zone, size_t index)
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

/*
**
*/

void				fix_table(struct s_zone *zone, size_t pivot)
{
	struct s_block	*next;
	struct s_block	*blk;
	//const size_t	cmp = zone->type == BLK_TINY ? BLOCK_MIN_SIZE : BLK_TINY_MAX;

	next = zone->block_table + pivot + 1;
	if (!pivot || pivot >= zone->table_size - 1 || !next->pointer)
		return ;
	blk = zone->block_table + pivot;
	// TODO: if pivot + 1 exists, then
	//           calculate the difference and if it is small, make current block bigger to fill the space
	//       else
	//           move pivot + 1 and create block with pointer 0
//	if (blk->pointer + blk->size - next->pointer >= cmp)
//	{
//		ft_putendl("moving memory");
//		ft_memmove(zone->block_table + pivot + 2, zone->block_table + pivot + 1, 
//					sizeof(struct s_block) * (zone->table_size - pivot - 1));
//	}
}

static inline struct s_block	*block_allocate(struct s_zone *zone, size_t idx, 
				size_t ptr, size_t size)
{
	struct s_block	*prev;
	struct s_block	*blk;

	ft_putstr("allocating block ");
	print_hex_nbr(ptr);
	ft_putendl("");
	prev = zone->block_table + idx - 1;
	blk = zone->block_table + idx;
	blk->pointer = ptr ? ptr : prev->pointer - size;
	blk->size = size;
	print_hex_dump(zone->block_table, 256, true);		
	if (!(blk->pointer > (size_t)(zone->block_table + zone->table_size) 
					&& blk->pointer <= (size_t)(zone + zone->zone_size)))
	{
		abort();
	};
	return (blk);
}

void							*get_block_reverse(struct s_zone *zone, size_t size)
{
	size_t			i;
	size_t			page_offset;
	size_t			desired_ptr;
	size_t			possible_block_idx;
	struct s_block	*table;

	possible_block_idx = 0;
	assert(size <= BLK_SMALL_MAX);
	assert((zone->type == BLK_TINY || zone->type == BLK_SMALL));
	if (size > BLK_SMALL_MAX || zone->zone_size - zone->bytes_malloced < size)
		return (NULL);
	table = zone->block_table;
	i = 0;
	while (++i < zone->table_size)
		if (table[i].pointer == 0)
		{
			desired_ptr = (size_t)table[i - 1].pointer - size;
			page_offset = desired_ptr & 0xfff;
			if (table[i + 1].pointer &&
				table[i + 1].pointer + table[i + 1].size < desired_ptr)
				continue ;
			if (page_offset + size >= g_storage->pagesize)
			{
				if (!possible_block_idx)
					possible_block_idx = i;
				continue ;
			}
			return ((void *)(block_allocate(zone, i, desired_ptr, size)->pointer));
		}
	ft_putstr(" hello from reverse allocator\n");
	return ((void *)block_allocate(zone, possible_block_idx, 0, size)->pointer);
}
