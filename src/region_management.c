/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   region_management.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 13:34:15 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/07 13:20:04 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <stdio.h>

bool					in_region_bounds(struct s_region *region, void *ptr)
{
	ft_putstr("region: ");
	ft_putnbr((long long)region);
	ft_putstr(", ");
	ft_putnbr((long long)region + region->bytes_mapped);
	ft_putchar('\n');
	if (ptr < (void *)region->start)
		return (false);
	if (ptr > (void *)((char *)region->start + region->bytes_mapped))
		return (false);
	return (true);
}

static struct s_zone	*find_space_for_zone(struct s_region *region, size_t size)
{
	struct s_zone	*zone = (region->start);

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
											enum e_size_type type, size_t size)
{
	struct s_zone	*list;
	struct s_zone	*zone;
	size_t			table_size;
	size_t			table_size_bytes;

	if (type <= 0 || type >= BLK_LARGE ||
		!(zone = find_space_for_zone(region, size)))
		return (NULL);
	table_size = size / (type == BLK_TINY ? BLOCK_MIN_SIZE : BLK_TINY_MAX);
	ft_putstr("  requested table size: ");
	ft_putnbr(table_size);
	ft_putendl("");
	table_size_bytes = table_size * sizeof(struct s_block);
	if (!(list = region->zones))
		region->zones = zone;
	else
	{
		while (list->next)
			list = list->next;
		list->next = zone;
	}
	*zone = (struct s_zone){ .zone_magic = ZONE_MAGIC,
		.type = type,
		.is_free = true,
		.table_size_age = 0,
		.bytes_malloced = 0,
		.bytes_free = size - table_size * sizeof(struct s_block),
		.zone_size = size,
		.table_size = table_size,
		.table_bound = 1,
		.first_free_block_index = 0,
		.next = NULL};
	ft_putstr("table size: ");
	ft_putnbr((uint64_t)zone->table_size);
	ft_putstr(" table address: ");
	print_hex_nbr((uint64_t)zone->block_table);
	ft_putchar('\n');
	ft_bzero(zone->block_table, table_size_bytes);
	zone->first_free_block_index = 1;
	zone->block_table[0].pointer = (size_t)zone + size;
	zone->block_table[0].size = 0;
	print_hex_dump(zone->block_table, 128, true);
	return (zone);
}
