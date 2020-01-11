/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:25:04 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/25 16:13:51 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"
#include <stdio.h>

pthread_mutex_t									g_mutex;
struct s_storage *restrict						g_storage = NULL;

struct s_region									*region_create(struct s_region *region, void *start, size_t size)
{
	ft_bzero(region, sizeof(struct s_region));
	region->start = start;
	region->bytes_mapped = size;
	return (start);
}

__attribute__((constructor)) static void	malloc_init(void)
{
	int					pagesize;
	size_t				first_region_size;

	pthread_mutex_init(&g_mutex, NULL);
	pagesize = getpagesize();
	first_region_size = 16384 + 524288 / 4;
	getrlimit(RLIMIT_DATA, &g_storage->stats.limits);
	g_storage = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	g_storage->map_start = g_storage;
	g_storage->pagesize = pagesize;
	g_storage->total_mapped = pagesize;
	g_storage->total_allocated = (void *)g_storage->regions - (void *)g_storage;
	write(1, "mapped some storage\n", 20);
	ft_bzero(g_storage->regions, sizeof(struct s_region));
	g_storage->regions[0].start = mmap(NULL, first_region_size,
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	region_create(g_storage->regions + 0, g_storage->regions[0].start,
					ALIGN_TO_PAGE(REGION_TINIES_SIZE, pagesize));
	printf("first region created\n");
	g_storage->total_mapped += first_region_size;
	g_storage->regions[1].start = region_create(g_storage->regions + 1,
		g_storage->regions[0].start + ALIGN_TO_PAGE(REGION_TINIES_SIZE, pagesize),
		ALIGN_TO_PAGE(REGION_SMALLIES_SIZE, pagesize));
	g_storage->regions_quantity = 2;
	region_create_zone(g_storage->regions + 0, BLK_TINY,
		ALIGN_TO_PAGE(REGION_TINIES_SIZE, pagesize));
	region_create_zone(g_storage->regions + 1, BLK_SMALL,
		ALIGN_TO_PAGE(REGION_SMALLIES_SIZE, pagesize));
}

__attribute__((destructor)) static void	malloc_destroy(void)
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
}

void											*malloc(size_t size)
{
	if (!g_storage)
		malloc_init();
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

