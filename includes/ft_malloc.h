/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:01:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/25 17:22:12 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

void	*malloc(size_t size);
void	*realloc(void *ptr, size_t new_size);
void	free(void *ptr);
void	show_alloc_mem(void);

#endif
