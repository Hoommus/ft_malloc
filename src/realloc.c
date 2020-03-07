/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/30 11:58:05 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/07 17:57:47 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"

size_t 			get_block_size(void *ptr)
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
			{
				return (g_storage->regions[i].bytes_mapped - sizeof(struct s_region));
			}
		}
	i = 0;
	while (zone && ++i < zone->table_size)
		if ((void *)zone->block_table[i].pointer == ptr)
			return (zone->block_table[i].size);
	return (false);

}

void __attribute__((visibility("default")))			*realloc(void *ptr, size_t new_size)
{
	void	*p;
	size_t	old_size;

	ft_putstr("\e[36;1m ***** realloc of ");
	print_hex_nbr((uint64_t)ptr);
	ft_putstr(" with new size ");
	ft_putnbr(new_size);
	ft_putendl("");
	if (!is_ptr_valid(ptr))
	{
		ft_putstr("      \e[32;1m** pointer is not valid\e[0m\n");
		return NULL;
	}
	if (!ptr)
		p = malloc(new_size);
	else if (new_size == 0)
		p = malloc(BLOCK_MIN_SIZE);
	else
	{
		old_size = get_block_size(ptr);
		ft_putstr("old size is: ");
		ft_putnbr(old_size);
		ft_putstr(" allocating new pointer ");
		if (new_size < old_size)
			old_size = new_size;
		p = malloc(new_size);
		ft_memcpy(p, ptr, old_size);
	}
	ft_putstr("\e[0m");
	return (p);
	// TODO: if new size > pagesize, don't try to realloc
	//       if new size >> this block size, try to realloc to bigger block
	//       if new size > 2^48, fail
	//       if ptr does not have enough space to the left to store block metadata, consider it a invalid address
	//       if ptr is not a valid block, fail
}
