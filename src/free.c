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

bool 			free_block(void *pointer)
{
	struct s_zone	*zone;
	size_t			i;

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
	i = -1;
	while (zone && ++i < zone->table_size)
		if ((void *)zone->block_table[i].pointer == pointer)
			return (zone->block_table[i].is_free = true);
	return (false);
}

void	free(void *ptr)
{
	if (!ptr)
		return ;
	if (!free_block(ptr))
	{
		//printf("**** pointer being freed was not allocated: %p\n", ptr);
		write(1, "hello from free \n", 16);
		ft_putnbr((int)ptr);
		ft_putchar('\n');
		abort();
	}
}
