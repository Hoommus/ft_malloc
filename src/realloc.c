/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/30 11:58:05 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/07 16:50:27 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

void			*realloc(void *ptr, size_t new_size)
{
	if (ptr_seems_valid(ptr))
		return NULL;
	(void)new_size;
	// TODO: if new size > pagesize, don't try to realloc
	//       if new size >> this block size, try to realloc to bigger block
	//       if new size > 2^48, fail
	//       if ptr does not have enough space to the left to store block metadata, consider it a invalid address
	//       if ptr is not a valid block, fail
	return (NULL);
}
