/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:34:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/25 15:48:07 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <assert.h>

#include <stdio.h>

void				*alloc_largie(size_t size)
{
	size_t				aligned;
	struct s_region		*region;

	aligned = ALIGN_TO_PAGE(ALIGN_TO_ARCH(sizeof(struct s_region)) + size, g_storage->pagesize);
	region = mmap(NULL, aligned, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
	ft_bzero(region, sizeof(region));
	region->start = region;
	region->bytes_mapped = aligned;
	region->bytes_malloced = size;
	region->is_free = false;
	region->is_full = aligned == size;
	region->large = (struct s_zone *)region + ALIGN_TO_ARCH(sizeof(struct s_region));
	pthread_mutex_lock(&g_mutex);
	g_storage->regions_quantity++;
	pthread_mutex_unlock(&g_mutex);
	return (region->large);
}

static size_t		next_free_block(struct s_zone *zone, size_t index)
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

void				*get_block(struct s_zone *zone, size_t size)
{
	size_t			i;
	size_t			page_offset;
	size_t			desired_ptr;
	struct s_block	*table;
	struct s_block	*possible_block;

	possible_block = NULL;
	assert(size <= g_storage->pagesize); // TODO: consider changing the strategy to `<= BLK_SMALL_MAX'
	assert((zone->type == BLK_TINY || zone->type == BLK_SMALL));
	if (size > g_storage->pagesize || zone->zone_size - zone->bytes_malloced < size)
		return (NULL);
	table = zone->block_table;
	i = 0;
	while (++i < zone->table_size - 1)
		if (table[i].pointer == 0 || table[i].is_free == true)
		{
			printf("this pointer: %zx, last %zx\n", table[i].pointer, table[i - 1].pointer);
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
			return ((void *)(table[i].pointer));
		}
	return (NULL);
}

void				*alloc(size_t size, enum e_size_type type)
{
	size_t			aligned;
	struct s_zone	*zone;
	struct s_region	*region;
	void			*blk;
	size_t			i;

	write(1, __func__, sizeof(__func__));
	blk = NULL;
	if (type == BLK_TINY)
		aligned = size < BLOCK_MIN_SIZE ? BLOCK_MIN_SIZE : ALIGN_TO_ARCH(size);
	else
		aligned = size + BLK_TINY_MAX >= BLK_SMALL_MAX ? BLK_SMALL_MAX : ALIGN_TO_ARCH(size);
	i = 0;
	pthread_mutex_lock(&g_mutex);
	printf("allocating block of size %zu(%zu)\n", size, aligned);
	region = g_storage->regions;
	while (i < g_storage->regions_quantity && !blk)
	{
		zone = region[i++].zones;
		while (zone)
		{
			if (!zone->is_full && (blk = get_block(zone, aligned)))
				break ;
			zone = zone->next;
		}
	}
	printf("mapped: %zu\n", g_storage->total_mapped);
	printf("total_allocated: %zu\n", g_storage->total_allocated);
	printf("allocated: %p\n", blk);
	pthread_mutex_unlock(&g_mutex);
	return (blk);
}
