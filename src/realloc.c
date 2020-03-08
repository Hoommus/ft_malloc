/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/30 11:58:05 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/08 16:14:53 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"

static struct s_zone	*get_block(void *ptr, size_t *idx)
{
	struct s_zone	*zone;
	size_t			i;

	if (!ptr)
		return 0;
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
				return ((struct s_zone *)(g_storage->regions + i));
		}
	*idx = 0;
	while (zone && ++(*idx) < zone->table_size)
		if ((void *)zone->block_table[*idx].pointer == ptr)
			return (zone);
	return (NULL);

}

void					*resize_ptr(struct s_zone *zone, size_t block_idx,
									void *ptr, size_t new_size)
{
	const struct s_block		*block = zone->block_table + block_idx;
	const struct s_block		*next = zone->block_table + block_idx + 1;
	void 						*p;

	if (block->size >= new_size)
		return (ptr);
	else if ((next->pointer && block->pointer + new_size < next->pointer)
		|| (block_idx == zone->table_bound))
	{
		((struct s_block *)block)->size = new_size;
		return (ptr);
	}
	else
	{
		p = ft_memcpy(malloc(new_size), (void *)block->pointer, block->size);
		free(ptr);
		return (p);
	}
}

void __attribute__((visibility("default")))
						*realloc(void *ptr, size_t new_size)
{
	void			*p;
	size_t			idx;
	struct s_zone	*zone;

	ft_putstr("\e[36;1m ***** realloc of ");
	print_hex_nbr((uint64_t)ptr);
	ft_putstr(" with new size ");
	ft_putnbr(new_size);
	ft_putendl("\e[0m");
	if (!is_ptr_valid(ptr))
		return (NULL);
	else if (!ptr)
		return (malloc(new_size));
	else if (new_size == 0)
		return (malloc(align_to(BLK_MIN_SIZE, 32)));
	zone = (struct s_zone *)get_block(ptr, &idx);
	if (!zone)
		return (NULL);
	if (zone->zone_magic == REGION_MAGIC)
	{
		if (((struct s_region *)zone)->bytes_mapped >= new_size)
			return (ptr);
		p = ft_memcpy(alloc_largie(new_size), ptr, ((struct s_region *)zone)->bytes_mapped);
		free(ptr);
		return (p);
	}
	else
		return (resize_ptr(zone, idx, ptr, new_size));
}
