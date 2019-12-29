/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   region_management.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:36:52 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/25 16:16:03 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <stdio.h>
//int				region_register(struct s_region *region)
//{
//	struct s_region		*list;
//
//	list = g_storage->regions;
//	if (!list)
//		return (!(g_storage->regions = region));
//	while (list->next)
//		list = list->next;
//	list->next = region;
//	return (0);
//}
//
//int				region_unregister(struct s_region *region)
//{
//	struct s_region		*swap;
//	struct s_region		*prev;
//	struct s_region		*list;
//
//	list = NULL;
//	prev = g_storage->regions;
//	if (prev)
//		list = prev->next;
//	if (prev && prev == region)
//		g_storage->regions = list;
//	while (list)
//	{
//		swap = list->next;
//		if (list == region)
//
//	}
//
//}

bool					in_region_bounds(struct s_region *region, void *ptr)
{
	if (ptr < (void *)region)
		return (false);
	if (ptr > (void *)region + region->bytes_mapped)
		return (false);
	return (true);
}

static struct s_zone	*find_space_for_zone(struct s_region *region, size_t size)
{
	struct s_zone	*zone = (region->start);

	printf("finding space for zone of size %zu at %p\n", size, zone);
	printf("region start %p, size %zu\n", region->start, region->bytes_mapped);
	while (((void *)zone < (void *)(region + region->bytes_mapped)) &&
		zone->zone_magic == ZONE_MAGIC) // in case region already has some zones
	{
		if (!in_region_bounds(region, zone))
			return (NULL);
		zone = zone + zone->zone_size;
	}
	printf("space found on %p\n", zone);
	zone->zone_size = size;
	return (zone);
}

struct s_zone			*region_create_zone(struct s_region *region,
											enum e_size_type type, size_t size)
{
	struct s_zone	*list;
	struct s_zone	*zone;
	size_t			table_size;

	if (type <= 0 || type >= BLK_LARGE ||
		!(zone = find_space_for_zone(region, size)))
		return (NULL);
	table_size = ((size / (type == BLK_TINY ? BLK_TINY_MAX : BLK_SMALL_MAX)) * 2) % SHRT_MAX;
	table_size = ALIGN_TO_PAGE(table_size * sizeof(struct s_block), g_storage->pagesize) / sizeof(struct s_block);
	if (!(list = region->zones))
		region->zones = zone;
	else
	{
		while (list->next)
			list = list->next;
		list->next = zone;
	}
	ft_bzero(zone, sizeof(struct s_zone));
	zone->zone_magic = ZONE_MAGIC;
	zone->table_size = table_size;
	printf("table size: %d\n", zone->table_size);
	printf("table address: %p\n", zone->block_table);
	zone->is_free = true;
	zone->type = type;
	zone->zone_size = size;
	zone->block_table[0].pointer = (size_t)zone + ALIGN_TO_ARCH(sizeof(*zone)) + zone->table_size * sizeof(struct s_block);
	zone->block_table[0].is_free = false;
	zone->block_table[0].size = 0;
	zone->first_free_block_index = 1;
	size_t		i;

	i = 1;
	while (i < zone->table_size)
	{
		ft_bzero(zone->block_table + i, sizeof(struct s_block));
		zone->block_table[i++].is_free = true;
	}
	return (zone);
}
