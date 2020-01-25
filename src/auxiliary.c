/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auxiliary.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <vtarasiu@student.unit.ua>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 16:11:30 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/01/25 14:13:14 by vtarasiu         ###   ########.fr       */
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

inline size_t __attribute__((always_inline)) align_to_page(size_t size, size_t pagesize)
{
	return (size + ABS((pagesize - size) % pagesize));
}
