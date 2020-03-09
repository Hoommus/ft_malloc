/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 14:59:18 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/09 18:38:56 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <assert.h>
#include <stdio.h>

static inline bool		free_large_region(struct s_region *region)
{
	int		ret;

	assert(!region->zones);
	assert(region->large);
	ret = munmap(region->start, region->bytes_mapped);
	ft_bzero(region, sizeof(struct s_region));
	return (!ret);
}

static inline bool		free_block(struct s_zone *zone, size_t idx)
{
	struct s_block	*blk;
	struct s_block	*next;

	zone->table_size_age++;
	blk = zone->block_table + idx;
	next = zone->block_table + idx + 1;
	blk->pointer = 0;
	if (idx >= zone->table_bound)
		blk->size = 0;
	else if (next->pointer == 0 && !next->size)
	{
		blk->size += next->size;
		next->size = 0;
		ft_memmove(next, next + 1,
			sizeof(struct s_block) * (zone->table_bound - 1));
	}
	zone->bytes_malloced -= blk->size;
	return (true);
}

bool					free_ptr(void *ptr)
{
	struct s_zone	*zone;
	size_t			i;

	zone = NULL;
	i = -1;
	while (++i < g_storage->regions_quantity)
		if (in_region_bounds(g_storage->regions + i, ptr))
		{
			if (!g_storage->regions[i].large)
			{
				zone = g_storage->regions[i].zones;
				break ;
			}
			else
				return (free_large_region(g_storage->regions + i));
		}
	i = 0;
	while (zone && ++i < zone->table_size)
		if ((void *)zone->block_table[i].pointer == ptr)
			return (free_block(zone, i));
	return (false);
}

void __attribute__((visibility("default")))					free(void *ptr)
{
	if (!ptr)
		return ;
	pthread_mutex_lock(&g_mutex);
	free_ptr(ptr);
	pthread_mutex_unlock(&g_mutex);
}
