/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auxiliary.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 16:11:30 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/07 17:23:54 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"

bool		ptr_seems_valid(void *ptr)
{
#if __POINTER_WIDTH__ == 64
	if (((size_t)ptr) & ADDRESS_SPACE_HEADER)
		return (false);
#endif
	if ((size_t)ptr % 8 != 0)
		return (false);
	return (true);
}
