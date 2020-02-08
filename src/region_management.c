/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   region_management.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <vtarasiu@student.unit.ua>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 13:34:15 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/02/08 13:53:19 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <stdio.h>

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
	if (g_storage->get_block == get_block_straight)
		table_size = ((size / (type == BLK_TINY ? BLK_TINY_MAX : BLK_SMALL_MAX))) % SHRT_MAX;
	else
		table_size = (size / (type == BLK_TINY ? BLOCK_MIN_SIZE : BLK_TINY_MAX)) % SHRT_MAX;
	table_size = ALIGN_TO_PAGE(table_size * sizeof(struct s_block), g_storage->pagesize) / sizeof(struct s_block);
	if (!(list = region->zones))
		region->zones = zone;
	else
	{
		while (list->next)
			list = list->next;
		list->next = zone;
	}
	*zone = (struct s_zone){ ZONE_MAGIC, type, false, true, 0, 0, size,
		table_size, 1, 0, 0, NULL };
	printf("table size: %d\n", zone->table_size);
	printf("table address: %p\n", zone->block_table);
	if (g_storage->get_block == get_block_straight)
		zone->block_table[0].pointer = (size_t)zone + ALIGN_TO_ARCH(sizeof(*zone)) + zone->table_size * sizeof(struct s_block);
	else
		zone->block_table[0].pointer = (size_t)zone + size;
	zone->block_table[0].is_free = false;
	zone->block_table[0].size = 0;
	ft_bzero(zone->block_table + 1, sizeof(struct s_block) * zone->table_size);
	return (zone);
}
