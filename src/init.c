/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <vtarasiu@student.unit.ua>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 16:55:23 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/01/25 16:57:14 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <stdio.h>

struct s_region									*region_create(struct s_region *region, void *start, size_t size)
{
	ft_bzero(region, sizeof(struct s_region));
	region->start = start;
	region->bytes_mapped = size;
	return (start);
}

static void										read_env(void)
{
	char	*var;

	g_storage->get_block = get_block_reverse;
	if ((var = getenv("FTMALLOC_THRESHOLD")) && ft_atoi(var) > 8)
		g_storage->threshold = ft_atoi_base(var, 10);
}

static void 									regions_init(size_t pagesize, size_t first_region_size)
{
	ft_bzero(g_storage->regions, pagesize * 2 - sizeof(*g_storage));
	g_storage->regions[0].start = mmap(NULL, first_region_size,
						PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	region_create(g_storage->regions + 0, g_storage->regions[0].start,
						align_to_page(REGION_TINIES_SIZE, pagesize));
	printf("first region created\n");
	g_storage->regions[1].start = region_create(g_storage->regions + 1,
		g_storage->regions[0].start + align_to_page(REGION_TINIES_SIZE, pagesize),
		align_to_page(REGION_SMALLIES_SIZE, pagesize));
	g_storage->regions_quantity = 2;	
	region_create_zone(g_storage->regions + 0, BLK_TINY,
					   align_to_page(REGION_TINIES_SIZE, pagesize));
	region_create_zone(g_storage->regions + 1, BLK_SMALL,
					   align_to_page(REGION_SMALLIES_SIZE, pagesize));
}

__attribute__((constructor,used)) static void	malloc_init(void)
{
	int					pagesize;
	size_t				first_region_size;

	ft_putstr("size of block: ");
	ft_putnbr(sizeof(struct s_block));
	ft_putendl("");
	pthread_mutex_init(&g_mutex, NULL);
	pagesize = getpagesize();
	first_region_size = REGION_TINIES_SIZE + REGION_SMALLIES_SIZE;
	getrlimit(RLIMIT_DATA, &g_storage->stats.limits);
	g_storage = mmap(0, pagesize * 2, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	g_storage->map_start = g_storage;
	g_storage->pagesize = pagesize;
	g_storage->total_mapped = pagesize * 2 + first_region_size;
	g_storage->total_allocated = (void *)g_storage->regions - (void *)g_storage;
	read_env();
	write(1, "mapped some storage\n", 20);
	regions_init(pagesize, first_region_size);
}
