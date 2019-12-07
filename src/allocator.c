/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   allocator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:34:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/07 18:49:08 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void		*alloc_largie(size_t size)
{
	size_t				aligned;
	struct s_region		*region;

	aligned = ALIGN_TO_PAGE(ALIGN_TO_ARCH(sizeof(struct s_region)) + size, g_storage->pagesize);
	region = mmap(NULL, aligned, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
	ft_bzero(region, sizeof(region));
	region->start = region;
	region->bytes_mapped = aligned;
	region->bytes_malloced = aligned;
	region->is_free = false;
	region->is_full = aligned == size;
	region->largies = (struct s_zone *)region + ALIGN_TO_ARCH(sizeof(struct s_region));
	return (region->largies);
}

void		*alloc_small(size_t size)
{

	const size_t	aligned = ALIGN_TO_ARCH(size);
	struct s_zone	*zone;


}

void		*alloc_tiny(size_t size)
{
	const size_t	aligned = size < BLOCK_MIN_SIZE ? BLOCK_MIN_SIZE : ALIGN_TO_ARCH(size);
}