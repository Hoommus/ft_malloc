/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   region_management.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 13:34:15 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/09 18:35:08 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <stdio.h>

bool					in_region_bounds(struct s_region *region, void *ptr)
{
	if (ptr < (void *)region->start)
		return (false);
	if (ptr > (void *)((char *)region->start + region->bytes_mapped))
		return (false);
	return (true);
}

static struct s_zone	*find_space_for_zone(struct s_region *region,
											size_t size)
{
	struct s_zone *zone;

	zone = (region->start);
	while (((void *)zone < (void *)(region + region->bytes_mapped)) &&
		zone->zone_magic == ZONE_MAGIC)
	{
		if (!in_region_bounds(region, zone))
			return (NULL);
		zone = zone + zone->zone_size;
	}
	zone->zone_size = size;
	return (zone);
}

struct s_zone			*region_create_zone(struct s_region *region,
											enum e_size_type type,
											size_t size)
{
	struct s_zone	*list;
	struct s_zone	*zone;
	size_t			table_size;
	size_t			table_size_bytes;

	if ((zone = find_space_for_zone(region, size)) == NULL)
		return (NULL);
	table_size = size / (type == BLK_TINY ? BLK_MIN_SIZE : BLK_TINY_MAX);
	table_size_bytes = table_size * sizeof(struct s_block);
	if (!(list = region->zones))
		region->zones = zone;
	else
	{
		while (list->next)
			list = list->next;
		list->next = zone;
	}
	*zone = (struct s_zone){ ZONE_MAGIC, type, true, 0, 0,
		size - table_size * sizeof(struct s_block),
		size, table_size, 1, 0, NULL};
	ft_bzero(zone->block_table, table_size_bytes);
	zone->first_free_block_index = 1;
	zone->block_table[0].pointer = (size_t)zone + size;
	zone->block_table[0].size = 0;
	return (zone);
}
