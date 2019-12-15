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

void		*get_block(struct s_zone *zone, size_t size)
{
	size_t		i;

	i = 1;
	while (i < zone->table_size)
	{
		if (zone->type == BLK_TINY && zone->block_table[i].pointer == 0)
		{
			zone->block_table[i].pointer = zone->block_table[i - 1].pointer + size;
			zone->block_table[i].size = size;
		}
		i++;
	}
	return (NULL);
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
			if ((blk = get_block(zone, size)))
				return (blk);
			zone = zone->next;
		}
		region = region->next;
	}
	return (NULL); // TODO: map new region
}