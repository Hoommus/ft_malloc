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
		if (!table[i].pointer)
			return (i);
		i++;
	}
	zone->is_full = true;
	return (i);
}

static void						insert_block(struct s_zone *zone, size_t pivot)
{
	struct s_block	*next;
	struct s_block	*blk;
	const size_t	cmp = zone->type == BLK_TINY ? BLOCK_MIN_SIZE : BLK_TINY_MAX;
	size_t			gap;

	next = zone->block_table + pivot + 1;
	if (!next->pointer)
		zone->idx_rightmost = pivot + 1;
	if (!pivot || pivot >= zone->table_size - 1 || !next->pointer)
		return ;
	blk = zone->block_table + pivot;
	// TODO: if pivot + 1 exists, then
	//           calculate the difference and if it is small, make current block bigger to fill up the space
	//       else
	//           move pivot + 1 and create block with pointer 0
	gap = blk->pointer + blk->size - next->pointer;
	if (gap >= cmp && zone->zone_size - zone->idx_rightmost > 0) // then make space for another block
	{
		ft_memmove(zone->block_table + pivot + 2, zone->block_table + pivot + 1, 
					sizeof(struct s_block) * (zone->table_size - pivot - 1));
		ft_bzero(zone->block_table + pivot + 1, sizeof(struct s_block));
	}
	else 
		blk->size = blk->size + gap;
}

int 							memcmp_pattern(const void *p, const void *pattern,
	size_t pattern_sizeof, size_t len)
{
	size_t		i;
	
	i = 0;
	if (pattern_sizeof > len)
		return (-1);
	while (i < len)
	{
		if (!ft_memcmp(p + pattern_sizeof, pattern, pattern_sizeof))
			return (1);
		p += pattern_sizeof;
	}
	return (0);
}

void				fix_table_size(struct s_zone *zone)
{
	size_t			table_size;
	size_t			min_table_size;
	const size_t	table_size_bytes = zone->table_size * sizeof(struct s_block);
	struct s_block	pattern;

	pattern = (struct s_block){0};
	if (zone->type == BLK_TINY)
	{
		table_size = table_size_bytes / BLOCK_MIN_SIZE;
		min_table_size = table_size_bytes / BLK_TINY_MAX;
	}
	else
	{
		table_size = table_size_bytes / BLK_TINY_MAX;	
		min_table_size = table_size_bytes / BLK_SMALL_MAX;
	}
	if (table_size > min_table_size && table_size < zone->table_size
		&& !memcmp_pattern(zone->block_table, &pattern, 
		sizeof(pattern), sizeof(pattern) * (zone->table_size - table_size)))
		zone->table_size = table_size;
	else if (table_size > min_table_size && table_size > zone->table_size)
		zone->table_size = table_size; // TODO: check for any allocated blocks in that memory part
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
	assert(blk->pointer > (size_t)(zone->block_table + zone->table_size) 
					&& blk->pointer <= (size_t)(zone + zone->zone_size));
	insert_block(zone, idx);
	if (zone->table_size_age < 8)
		zone->table_size_age++;
	else
		fix_table_size(zone);
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
			// TODO: check if there is space
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
