/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:34:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/15 19:41:20 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <assert.h>

void		*alloc_largie(size_t size)
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
	return (region->large);
}

void		*alloc_small(size_t size)
{
	const size_t	aligned = ALIGN_TO_ARCH(size);
	struct s_zone	*zone;


}

size_t 		next_free_block(struct s_zone *zone, size_t index)
{
	size_t			i;
	struct s_block	*table;
	
	if (zone->block_table[index + 1].pointer == 0)
		return (index + 1);
	table = zone->block_table;
	i = index;
	while (i < zone->table_size)
	{
		
		i++;
	}
	return (i);
}

void		*get_block(struct s_zone *zone, size_t size)
{
	size_t			i;
	size_t 			page_offset;
	size_t			desired_ptr;
	struct s_block	*table;

	assert(size <= g_storage->pagesize);
	if (size > g_storage->pagesize || zone->zone_size - zone->bytes_malloced < size)
		return (NULL);
	table = zone->block_table;
	i = zone->first_free_block - 1;
	while (++i < zone->table_size - 1)
		if ((zone->type == BLK_TINY || zone->type == BLK_SMALL) && table[i].pointer == 0)
		{
			desired_ptr = table[i - 1].pointer + size;
			page_offset = table[i].pointer & PAGE_ADDRESS_MASK;
			if ((size <= g_storage->pagesize && page_offset + size >= g_storage->pagesize)
				|| (table[i + 1].pointer && table[i + 1].pointer < desired_ptr))
				continue ;
			table[i].pointer = table[i - 1].pointer + size;
			table[i].size = size;
			// TODO: mark next free block or set zone as full
			return ((void *)(0x0 | table[i].pointer));
		}
	return (NULL);
}

void 		*alloc(size_t size, enum e_size_type type)
{

}

void		*alloc_tiny(size_t size)
{
	const size_t	aligned = size < BLOCK_MIN_SIZE ? BLOCK_MIN_SIZE
													: ALIGN_TO_ARCH(size);
	struct s_zone	*zone;
	struct s_region	*region;
	void			*blk;

	region = g_storage->regions;
	while (region)
	{
		zone = region->zones;
		while (zone)
		{
			if (!zone->is_full && (blk = get_block(zone, aligned)))
				return (blk);
			zone = zone->next;
		}
		region = region->next;
	}
	return (NULL); // TODO: map new region
}
