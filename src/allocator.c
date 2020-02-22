/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:34:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/25 15:48:07 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"
#include <stdio.h>
#include <assert.h>

void				*alloc_largie(size_t size)
{
	size_t				aligned;
	struct s_region		*region;

	aligned = ALIGN_TO_PAGE(ALIGN_TO_ARCH(sizeof(struct s_region)) + size, g_storage->pagesize);
	region = mmap(NULL, aligned, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	ft_bzero(region, sizeof(region));
	region->start = region;
	region->bytes_mapped = aligned;
	region->bytes_malloced = size;
//	region->is_full = aligned == size;
	region->large = (struct s_zone *)region + ALIGN_TO_ARCH(sizeof(struct s_region));
	pthread_mutex_lock(&g_mutex);
	g_storage->regions_quantity++;
	g_storage->regions[g_storage->regions_quantity - 1] = *region;
	pthread_mutex_unlock(&g_mutex);
	return (region->large);
}

void				*alloc(size_t size, enum e_size_type type)
{
	size_t			aligned;
	struct s_zone	*zone;
	struct s_region	*region;
	void			*blk;
	size_t			i;

	blk = NULL;
	if (type == BLK_TINY)
		aligned = size < BLOCK_MIN_SIZE ? BLOCK_MIN_SIZE : ALIGN_TO_ARCH(size);
	else
		aligned = size + BLK_TINY_MAX >= BLK_SMALL_MAX ? BLK_SMALL_MAX : ALIGN_TO_ARCH(size);
	i = 0;
	write(1, "Locking mutex\n", 14);
	pthread_mutex_lock(&g_mutex);
	write(1, "Inside alloc 2\n", 15);
	//printf("allocating block of size %zu(%zu)\n", size, aligned);
	region = g_storage->regions;
	while (i < g_storage->regions_quantity && !blk)
	{
		zone = region[i++].zones;
		while (zone)
		{
			if (!zone->is_full && (blk = g_storage->get_block(zone, aligned)))
				break ;
			zone = zone->next;
		}
	}
//	printf("mapped: %zu\n", g_storage->total_mapped);
//	printf("total_allocated: %zu\n", g_storage->total_allocated);
//	printf("allocated: %p\n", blk);
	write(1, "allocated something ", 20);
	ft_putnbr_fd((long long) blk, 1);
	write(1, "\n", 1);
	pthread_mutex_unlock(&g_mutex);
	assert((size_t)blk % 8 == 0);
	return (blk);
}
