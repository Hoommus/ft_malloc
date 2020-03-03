/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 14:59:18 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/25 16:20:23 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <assert.h>
#include <stdio.h>

bool			free_large_region(struct s_region *region)
{
	int		ret;
	
	assert(!region->zones);
	assert(region->large);
	assert(!region->large->next);
	ret = munmap(region->start, region->bytes_mapped);
	ft_bzero(region, sizeof(struct s_region));
	return (!ret);
}

static bool		free_block(struct s_block *blk)
{
	blk->pointer = 0;
	blk->size = 0;
	return (true);
}

// TODO: add table_size_age alterations
bool 			free_ptr(void *pointer)
{
	struct s_zone	*zone;
	size_t			i;
	void 			*cmp;

	zone = NULL;
	i = -1;
	while (++i < g_storage->regions_quantity)
		if (in_region_bounds(g_storage->regions + i, pointer))
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
	{
		if (zone->block_table[i].pointer != 0)
		{
			ft_putstr("  trying to free ");
			print_hex_nbr((uint64_t)pointer);
			ft_putstr(" against ");
			print_hex_nbr(zone->block_table[i].pointer);
			ft_putendl("");
		}
		cmp = (void *)zone->block_table[i].pointer;
		if (cmp == pointer)
			return (free_block(zone->block_table + i));
	}
	return (false);
}

void	free(void *ptr)
{
	if (!ptr)
		return ;
	if (!free_ptr(ptr))
	{
		ft_putstr(" - error from free\n");
		print_hex_nbr((uint64_t)ptr);
		ft_putchar('\n');
	}
}
