/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   region_management.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:36:52 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/15 17:58:25 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

int				region_register(struct s_region *region)
{
	struct s_region		*list;

	list = g_storage->regions;
	if (!list)
		return (!(g_storage->regions = region));
	while (list->next)
		list = list->next;
	list->next = region;
	return (0);
}

int				region_unregister(struct s_region *region)
{
	struct s_region		*swap;
	struct s_region		*prev;
	struct s_region		*list;

	list = NULL;
	prev = g_storage->regions;
	if (prev)
		list = prev->next;
	if (prev && prev == region)
		g_storage->regions = list;
	while (list)
	{
		swap = list->next;
		if (list == region)

	}


}

bool			in_region_bounds(struct s_region *region, void *ptr)
{
	if (ptr < (void *)region || ptr > (void *)region + region->bytes_mapped)
		return (false);
	return (true);
}

struct s_zone	*find_space_for_zone(struct s_region *region, size_t size)
{
	struct s_zone	*zone = (region->start + ALIGN_TO_ARCH(sizeof(struct s_region)));

	while (zone->zone_magic == ZONE_MAGIC)
	{
		if (!in_region_bounds(region, zone))
			return (NULL);
		zone = zone + zone->zone_size;
	}
	if (in_region_bounds(region, zone) && in_region_bounds(region, zone + size))
		return (zone);
	return (NULL);
}

struct s_zone	*region_create_zone(struct s_region *region, enum e_size_type type, size_t size)
{
	struct s_zone	*list;
	struct s_zone	*zone = (region->start + ALIGN_TO_ARCH(sizeof(struct s_region)));
	size_t			table_size;

	if (type <= 0 || type >= BLK_LARGE ||
		!(zone = find_space_for_zone(region, size)))
		return (NULL);
	table_size = size / (type == BLK_TINY ? BLK_TINY_MAX : BLK_SMALL_MAX) / 4;
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
	zone->is_free = true;
}
