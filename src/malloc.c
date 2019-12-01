/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:25:04 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/01 18:19:03 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

pthread_mutex_t								g_mutex;
struct s_storage *restrict					g_storage;

struct s_region								*region_create(void *start, size_t size, enum e_size_type type)
{
	struct s_region		*region;

	region = start;
	ft_bzero(region, sizeof(struct s_region));
	region->start = start;
	region->bytes_mapped = size;
	if (type == BLK_TINY)
		region->tinies = region->start + ALIGN_TO_ARCH( sizeof(struct s_region));
	else if (type == BLK_SMALL)
		region->smallies = region->start + sizeof(struct s_region);
	else
		region->largies = region->start + sizeof(struct s_region);
	return (region);
}

__attribute__((constructor)) static void	malloc_init(void)
{
	int				pagesize;
	const size_t	first_region_size =
		ALIGN_TO_PAGE(REGION_TINIES_SIZE + REGION_SMALLIES_SIZE, pagesize);
	struct rlimit	lim;

	pthread_mutex_init(&g_mutex, NULL);
	pagesize = getpagesize();
	getrlimit(RLIMIT_DATA, &lim);
	g_storage = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
	g_storage->map_start = g_storage;
	g_storage->pagesize = pagesize;
	g_storage->rlim_cur = lim.rlim_cur;
	g_storage->rlim_max = lim.rlim_max;
	g_storage->total_mapped = pagesize;
	g_storage->regions = (struct s_region *)(g_storage + ALIGN_TO_ARCH(sizeof(*g_storage)));
	g_storage->total_allocated = (void *)g_storage - (void *)g_storage->regions;
	ft_bzero(g_storage->regions, sizeof(struct s_region));
	g_storage->regions->next = mmap(NULL, first_region_size,
		PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
	g_storage->regions->next = region_create(g_storage->regions->next, REGION_TINIES_SIZE, BLK_TINY);
	//g_storage->regions->next = region_create(g_storage->regions->next, REGION_SMALLIES_SIZE, BLK_SMALL);
	//g_storage->regions->next->next = g_storage->regions->next + REGION_TINIES_SIZE;
}

__attribute__((destructor)) static void		malloc_destroy(void)
{
	struct s_region		*list;
	struct s_region		*next;

	pthread_mutex_lock(&g_mutex);
	list = g_storage->regions->next;
	while (list)
	{
		next = list->next;
		munmap(list->start, list->bytes_mapped);
		list = next;
	}
	munmap(g_storage->map_start, g_storage->pagesize);

	pthread_mutex_unlock(&g_mutex);
	pthread_mutex_destroy(&g_mutex);
}


void										*malloc(size_t size)
{
	// if size > pagesize, map a large zone
	;
}
