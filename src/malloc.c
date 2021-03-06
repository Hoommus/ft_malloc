/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:25:04 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/09 18:40:55 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"

#include <stdio.h>

pthread_mutex_t				g_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
struct s_storage *restrict	g_storage = NULL;

static void __attribute__((destructor))
							malloc_destroy(void)
{
	struct s_region		*array;
	struct s_zone		*zones;
	size_t				i;

	if (!g_storage)
		return ;
	i = 0;
	pthread_mutex_lock(&g_mutex);
	array = g_storage->regions;
	while (i < g_storage->regions_quantity)
	{
		zones = array[i].zones;
		while (zones)
		{
			if (zones->zone_magic == ZONE_MAGIC)
				zones->zone_magic = 0xbe5fbe5fbe5fbe5f;
			zones = zones->next;
		}
		munmap(array[i].start, array[i].bytes_mapped);
		i++;
	}
	munmap(g_storage->map_start, g_storage->pagesize);
	g_storage = NULL;
	pthread_mutex_unlock(&g_mutex);
	pthread_mutex_destroy(&g_mutex);
}

void __attribute__((visibility("default")))
							*malloc(size_t size)
{
	if (!g_storage)
		malloc_init();
	if (size == 0)
		return (NULL);
	if (size > BLK_SMALL_MAX)
		return (alloc_largie(size));
	if (size > BLK_TINY_MAX)
		return (alloc(size, BLK_SMALL));
	return (alloc(size, BLK_TINY));
}
