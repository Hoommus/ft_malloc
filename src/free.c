/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 14:59:18 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/07 18:14:20 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void	free(void *ptr)
{
	if (!ptr)
		return ;
	if (!ptr_seems_valid(ptr))
	{
		ft_dprintf(2, "*** pointer being freed was not allocated: %p\n", ptr);
		abort();
	}
	// search through regions using page numbers from PAGE_NUMBER_MASK macro
}
