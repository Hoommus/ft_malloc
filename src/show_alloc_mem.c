/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/10 20:00:28 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/10 21:11:41 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"

void		show_zone_table(const struct s_zone *zone)
{
	const struct s_block	*table = zone->block_table;
	size_t			i;

	i = zone->table_bound;
	while (i > 0)
	{
		if (table[i].pointer)
		{
			ft_putstr("\t");
			print_hex_nbr(table[i].pointer);
			ft_putstr(" - ");
			print_hex_nbr(table[i].pointer + table[i].size);
			ft_putstr(" (size ");
			ft_putnbr(table[i].size);
			ft_putendl(")");
		}
		i--;
	}
}

void		show_zone(const struct s_zone *zone)
{
	ft_putstr(zone->type == BLK_TINY ? "TINY zone " : "SMALL zone :");
	print_hex_nbr((uint64_t)(zone->block_table + zone->table_size));
	ft_putstr(" - ");
	print_hex_nbr((uint64_t)zone + zone->zone_size);
	ft_putendl("");
	show_zone_table(zone);
}

void		show_alloc_mem_ex(void)
{

}

void		show_alloc_mem(void)
{
	size_t		i;

	i = 0;
	while (i < g_storage->regions_quantity)
	{
		ft_putstr("REGION #");
		ft_putnbr(i);
		ft_putstr("\n");
		i++;
	}
}
