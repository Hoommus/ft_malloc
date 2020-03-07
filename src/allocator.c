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

#include "ft_malloc_private.h"
#include <stdio.h>

void				*alloc_largie(size_t size)
{
	size_t				aligned;
	struct s_region		*region;

	aligned = ALIGN_TO_PAGE(ALIGN_TO_ARCH(sizeof(struct s_region)) + size, g_storage->pagesize);
	region = mmap(NULL, aligned, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	ft_bzero(region, sizeof(region));
	region->start = region;
	region->bytes_mapped = aligned;
	region->bytes_malloced = size;
//	region->is_full = aligned == size;
	region->large = (struct s_zone *)region + ALIGN_TO_ARCH(sizeof(struct s_region));
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

	region_size = align_to_page(sizeof(struct s_region) + (type == BLK_TINY 
			? REGION_TINIES_SIZE : REGION_SMALLIES_SIZE), g_storage->pagesize);
	new_region = mmap(NULL, region_size, 
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0); 
	region_create(new_region, new_region, region_size);
	return (alloc(size, type));
}

#include <sys/types.h>
#include <signal.h>
void				*alloc(size_t size, enum e_size_type type)
{
	size_t			aligned;
	struct s_zone	*zone;
	struct s_region	*region;
	void			*blk;
	size_t			i;

	blk = NULL;
	if (type == BLK_TINY)
		aligned = size < BLOCK_MIN_SIZE ? BLOCK_MIN_SIZE : ALIGN_TO_ARCH(size);
	else
		aligned = size + BLK_TINY_MAX >= BLK_SMALL_MAX ? BLK_SMALL_MAX : ALIGN_TO_ARCH(size);
	i = 0;
	pthread_mutex_lock(&g_mutex);
	region = g_storage->regions;
	while (i < g_storage->regions_quantity && !blk)
	{
		zone = region[i++].zones;
		while (zone && zone->type == type)
		{
			ft_putstr("zone chosen, looking for block\nzone: ");
			print_hex_nbr((uint64_t)zone);
			ft_putendl("");
			if (zone->zone_magic != ZONE_MAGIC)
				kill(0, 1);
			if (!zone->is_full && (blk = get_block_reverse(zone, aligned)))
				break ;
			zone = zone->next;
		}
	}
	if (!blk)
		blk = mmap_more_and_alloc(size, type);
	pthread_mutex_unlock(&g_mutex);
	return (blk);
}
