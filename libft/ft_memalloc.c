/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memalloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/29 13:55:19 by vtarasiu          #+#    #+#             */
/*   Updated: 2018/03/29 14:02:23 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memalloc(size_t size)
{
	unsigned char	*ptr;

	ptr = (unsigned char *)malloc(sizeof(unsigned char) * size);
	if (ptr == NULL)
		return (NULL);
	ft_bzero(ptr, size);
	return ((void *)ptr);
}
