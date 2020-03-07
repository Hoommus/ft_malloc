/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator_reverse.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 16:54:58 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/07 18:12:09 by vtarasiu         ###   ########.fr       */
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
	if (gap >= cmp && zone->table_bound - pivot > 2)
	{
		ft_memmove(zone->block_table + pivot + 2, zone->block_table + pivot + 1, 
					sizeof(struct s_block) * (zone->table_size - pivot - 1));
		ft_bzero(zone->block_table + pivot + 1, sizeof(struct s_block));
	}
	else 
		blk->size = blk->size + gap;
}

int						memcmp_byte(const void *p, uint8_t byte, size_t len)
{
	while (len--)
		if (*((uint8_t *)p) != byte)
			return (*(uint8_t *)p - byte);
	return (0);
}

static void				fix_table_size(struct s_zone *zone)
{
	size_t			min_table_size;
	size_t			new_table_size;
	size_t			free_space;

	if (zone->table_size_age < 8)
		return ;
	ft_putstr("+++++ fixing table size from ");
	ft_putnbr(zone->table_size);
	free_space = zone->bytes_free;
	if (zone->type == BLK_TINY)
	{
		min_table_size = free_space / BLK_TINY_MAX;
		new_table_size = free_space / BLK_MIN_SIZE;
	}
	else
	{
		min_table_size = free_space / BLK_SMALL_MAX;
		new_table_size = free_space / BLK_TINY_MAX;
	}
	new_table_size += zone->first_free_block_index;
	if (new_table_size < min_table_size)
		return ;
	if (new_table_size < zone->table_size &&
		!memcmp_byte(zone->block_table + new_table_size, 0,
		sizeof(struct s_block) * (zone->table_size - new_table_size - 1)))
		zone->table_size = new_table_size;
	else if (new_table_size > zone->table_size
			 && !memcmp_byte(zone->block_table + zone->table_size, 0,
			sizeof(struct s_block) * (new_table_size - zone->table_size)))
		zone->table_size = new_table_size;
	zone->table_size_age = 0;
	ft_putstr(" to ");
	ft_putnbr(zone->table_size);
	ft_putendl("");
}

static void				place_bounds(struct s_zone *zone, size_t pivot)
{
	const struct s_block	*table = zone->block_table;
	const size_t			size = zone->table_size;

	ft_putstr("==== old bounds: ");
	ft_putnbr(pivot);
	ft_putstr(" first free block: ");
	ft_putnbr(zone->first_free_block_index);
	ft_putstr(" table bound: ");
	ft_putnbr(zone->table_bound);
	ft_putendl("");
	while (++pivot < size)
		if (table[pivot].pointer == 0 && pivot < zone->table_bound)
		{
			zone->first_free_block_index = pivot;
			break ;
		}
	while (++pivot < size)
		if (table[pivot].pointer == 0 && pivot > zone->first_free_block_index &&
			pivot < zone->table_size)
		{
			zone->table_bound = pivot;
			break ;
		}
	zone->bytes_free = zone->block_table[zone->table_bound - 1].pointer -
					   (size_t)zone->block_table + zone->table_bound;
	ft_putstr("==== new bounds: ");
	ft_putstr(" first free block: ");
	ft_putnbr(zone->first_free_block_index);
	ft_putstr(" table bound: ");
	ft_putnbr(zone->table_bound);
	ft_putstr(" free bytes: ");
	ft_putnbr(zone->bytes_free);
	ft_putendl("");
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
	ft_memcpy((void *)blk->pointer, "hello from block_allocate", sizeof("hello from block_allocate"));
	ft_putstr("allocating block ");
	print_hex_nbr(blk->pointer);
	ft_putstr(" with size ");
	ft_putnbr(size);
	ft_putendl("");
	//print_hex_dump(zone->block_table, 256, true);		
	if (blk->pointer <= (size_t)(zone->block_table + zone->table_size)
					&& blk->pointer > (size_t)zone + zone->zone_size)
		ft_putstr("\e[37;1m some invalid pointer allocated\e[0m\n");
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
	i = zone->first_free_block_index - 1;
	while (++i < zone->table_size)
		if (table[i].pointer == 0)
		{
			desired_ptr = (size_t)table[i - 1].pointer - size;
			page_offset = desired_ptr & 0xfff;
			if (table[i + 1].pointer &&
				table[i + 1].pointer + table[i + 1].size < desired_ptr)
				continue ;
			if ((size_t)(table + zone->table_size) >= desired_ptr)
				break ;
			if (page_offset + size >= g_storage->pagesize)
			{
				if (!possible_block_idx)
					possible_block_idx = i;
				continue ;
			}
			return (block_allocate(zone, i, desired_ptr, size));
		}
	return (block_allocate(zone, possible_block_idx, 0, size));
}
