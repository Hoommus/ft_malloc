/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator_reverse.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 16:54:58 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/09 18:29:32 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <assert.h>

static void				insert_block(struct s_zone *zone, size_t pivot)
{
	struct s_block	*next;
	struct s_block	*blk;
	const size_t	cmp = zone->type == BLK_TINY ? BLK_MIN_SIZE : BLK_TINY_MAX;
	size_t			gap;

	next = zone->block_table + pivot + 1;
	if (!pivot || pivot >= zone->table_size - 1 || !next->pointer)
		return ;
	blk = zone->block_table + pivot;
	gap = blk->pointer + blk->size - next->pointer;
	if (gap >= cmp && zone->table_bound > pivot + 2)
	{
		ft_memmove(zone->block_table + pivot + 2, zone->block_table + pivot + 1,
					sizeof(struct s_block) * (zone->table_size - pivot - 1));
		ft_bzero(zone->block_table + pivot + 1, sizeof(struct s_block));
	}
	else
		blk->size = blk->size + gap;
}

static void				fix_table_size(struct s_zone *zone)
{
	size_t			min_table_size;
	size_t			new_table_size;
	size_t			free_space;

	if (zone->table_size_age < 8)
		return ;
	free_space = zone->bytes_free;
	min_table_size = free_space /
		(zone->type == BLK_TINY ? BLK_TINY_MAX : BLK_SMALL_MAX);
	new_table_size = free_space /
		(zone->type == BLK_TINY ? BLK_MIN_SIZE : BLK_TINY_MAX);
	if (new_table_size < min_table_size)
		return ;
	if (new_table_size > zone->table_bound)
		zone->table_size = new_table_size;
	zone->table_size_age = 0;
}

void					place_bounds(struct s_zone *zone, size_t pivot)
{
	const struct s_block	*table = zone->block_table;
	const size_t			size = zone->table_size;
	size_t					i;

	if (zone->table_size_age % 2 != 0)
		return ;
	while (pivot < size && table[pivot].pointer != 0)
		pivot++;
	zone->first_free_block_index = pivot;
	i = size - 1;
	while (i > zone->first_free_block_index && table[i].pointer == 0)
		i--;
	zone->table_bound = i;
	zone->bytes_free = table[i - 1].pointer - (size_t)(table + size);
}

static void				*block_allocate(struct s_zone *zone, size_t idx,
				size_t ptr, size_t size)
{
	struct s_block	*prev;
	struct s_block	*blk;

	if (idx == 0)
		return (NULL);
	prev = zone->block_table + idx - 1;
	blk = zone->block_table + idx;
	blk->pointer = ptr ? ptr : prev->pointer - size;
	blk->size = size;
	insert_block(zone, idx);
	place_bounds(zone, idx);
	zone->bytes_malloced += blk->size;
	if (zone->table_size_age < 8)
		zone->table_size_age++;
	else
		fix_table_size(zone);
	return ((void *)blk->pointer);
}

void					*get_block_reverse(struct s_zone *zone, size_t size)
{
	size_t			i;
	size_t			desired_ptr;
	struct s_block	*table;

	if (size > BLK_SMALL_MAX || zone->zone_size - zone->bytes_malloced < size)
		return (NULL);
	table = zone->block_table;
	i = 0;
	while (++i < zone->table_size)
		if (table[i].pointer == 0)
		{
			desired_ptr = (size_t)table[i - 1].pointer - size;
			if (table[i + 1].pointer &&
				table[i + 1].pointer + table[i + 1].size < desired_ptr)
				continue ;
			if ((size_t)(table + zone->table_size) >= desired_ptr)
				break ;
			return (block_allocate(zone, i, desired_ptr, size));
		}
	return (NULL);
}
