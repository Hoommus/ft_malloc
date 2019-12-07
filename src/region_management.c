/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   region_management.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:36:52 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/07 18:17:10 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

int			region_register(struct s_region *region)
{
	struct s_region		*list;

	list = g_storage->regions;
	if (!list)
		return (!(g_storage->regions = region));
	while (list->next)
		list = list->next;
	list->next = region;
	return (0);
}

int			region_unregister(struct s_region *region)
{
	struct s_region		*swap;
	struct s_region		*prev;
	struct s_region		*list;

	list = NULL;
	prev = g_storage->regions;
	if (prev)
		list = prev->next;
	if (prev && prev == region)
		g_storage->regions = list;
	while (list)
	{
		swap = list->next;
		if (list == region)

	}


}
