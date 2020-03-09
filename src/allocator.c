/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:34:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/09 18:28:31 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <stdio.h>
#include <assert.h>

void				*alloc_largie(size_t size)
{
	size_t				aligned;
	struct s_region		*region;

	aligned = align_to(align_to(sizeof(struct s_region), ALIGNMENT) + size,
		g_storage->pagesize);
	region = mmap(NULL, aligned, PROT_READ | PROT_WRITE,
								MAP_ANON | MAP_PRIVATE, -1, 0);
	ft_bzero(region, sizeof(*region));
	region->start = region;
	region->bytes_mapped = aligned;
	region->large = (void *)((size_t)region + align_to(sizeof(struct s_region), 16));
	pthread_mutex_lock(&g_mutex);
	g_storage->regions_quantity++;
	g_storage->regions[g_storage->regions_quantity - 1] = *region;
	pthread_mutex_unlock(&g_mutex);
	return (region->large);
}

static void 		*mmap_more_and_alloc(size_t size, enum e_size_type type)
{
	struct s_region	*new_region;
	size_t 			region_size;

	region_size = align_to(sizeof(struct s_region) +
		(type == BLK_TINY ? REGION_TINIES_SIZE : REGION_SMALLIES_SIZE),
		g_storage->pagesize);
	new_region = g_storage->regions + g_storage->regions_quantity;
	ft_bzero(new_region, sizeof(*new_region));
	new_region->start = mmap(NULL, region_size,
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	region_create(new_region, new_region->start, region_size);
	region_create_zone(new_region, type, region_size);
	g_storage->regions_quantity++;
	return (alloc(size, type));
}

void				*alloc(size_t size, enum e_size_type type)
{
	size_t			aligned;
	struct s_zone	*zone;
	struct s_region	*region;
	void			*blk;
	size_t			i;

	blk = NULL;
	if (type == BLK_TINY)
		aligned = size < BLK_MIN_SIZE ? BLK_MIN_SIZE : align_to(size, 8);
	else
		aligned = size + BLK_TINY_MAX >= BLK_SMALL_MAX ? BLK_SMALL_MAX : align_to(size, 8);
	i = 0;
	pthread_mutex_lock(&g_mutex);
	region = g_storage->regions;
	while (i < g_storage->regions_quantity && !blk)
	{
		zone = region[i++].zones;
		while (zone && zone->type == type)
		{
			if (zone->bytes_free && (blk = get_block_reverse(zone, aligned)))
				break ;
			zone = zone->next;
		}
	}
	if (!blk)
		blk = mmap_more_and_alloc(size, type);
	pthread_mutex_unlock(&g_mutex);
	return (blk);
}
