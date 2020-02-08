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

size_t		next_free_block(struct s_zone *zone, size_t index)
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

static inline void					fix_table(struct s_zone *zone, size_t pivot)
{
	struct s_block	*next;
	struct s_block	*blk;
	const size_t	cmp = zone->type == BLK_TINY ? BLOCK_MIN_SIZE : BLK_TINY_MAX;

	next = zone->block_table + pivot + 1;
	if (!pivot || pivot >= zone->table_size - 1 || next->pointer)
		return ;
	blk = zone->block_table + pivot;
	if (blk->pointer + next->size - next->pointer >= cmp)
		ft_memmove(zone->block_table + pivot + 2, zone->block_table + pivot + 1, 
						sizeof(struct s_block) * (zone->table_size - pivot - 1));
}

static inline struct s_block		*block_allocate(struct s_zone *zone, size_t idx, size_t size)
{
	struct s_block	*prev;
	struct s_block	*blk;

	prev = zone->block_table + idx - 1;
	blk = zone->block_table + idx;
	blk->pointer = prev->pointer - prev->size - size;
	blk->size = size;
	fix_table(zone, idx);
	return (blk);
}

void			*get_block_reverse(struct s_zone *zone, size_t size)
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
	while (++i)
		if (table[i].pointer == 0)
		{
			write(1, "hello\n", 6);
			desired_ptr = table[i - 1].pointer - table[i - 1].size - size;
			page_offset = (table[i - 1].pointer - table[i - 1].size) & PAGE_ADDRESS_MASK;
			if (table[i + 1].pointer && table[i + 1].pointer < desired_ptr)
				continue ;
			if (page_offset - size >= g_storage->pagesize)
			{
				if (!possible_block_idx)
					possible_block_idx = i;
				continue ;
			}
			return ((void *)block_allocate(zone, i, size));
		}
	ft_putstr("hello\n");
	return ((void *)block_allocate(zone, possible_block_idx, size));
}
