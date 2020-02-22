/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <vtarasiu@student.unit.ua>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:25:04 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/02/22 17:34:24 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"

#include <stdio.h>

pthread_mutex_t									g_mutex;
struct s_storage *restrict						g_storage = NULL;

__attribute__((destructor,used)) static void	malloc_destroy(void)
{
	struct s_region		*array;
	struct s_zone		*zones;
	size_t				i;

	i = 0;
	write(1, "destroyin\n", 11);
	pthread_mutex_lock(&g_mutex);
	array = g_storage->regions;
	while (i < g_storage->regions_quantity)
	{
		zones = array[i].zones;
		while (zones)
		{
			if (zones->zone_magic == ZONE_MAGIC)
				zones->zone_magic = (typeof(zones->zone_magic))0xbe5fbe5fbe5fbe5f;
			zones = zones->next;
		}
		munmap(array[i].start, array[i].bytes_mapped);
		i++;
	}
	munmap(g_storage->map_start, g_storage->pagesize);
	pthread_mutex_unlock(&g_mutex);
	pthread_mutex_destroy(&g_mutex);
	ft_putstr("destroyed\n");
}

void											*malloc(size_t size)
{
	if (!g_storage)
		malloc_init();
	ft_putstr("inside our malloc\n");
	if (size == 0)
		return (NULL);
	if (size > BLK_SMALL_MAX)
		return (alloc_largie(size));
	if (size > BLK_TINY_MAX)
		return (alloc(size, BLK_SMALL));
	//printf("mapped: %zu\n", g_storage->total_mapped);
	//printf("total allocated: %zu\n", g_storage->total_allocated);
	return (alloc(size, BLK_TINY));
}

