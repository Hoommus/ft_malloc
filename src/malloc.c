/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:25:04 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/04/04 15:25:04 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc.h"

pthread_mutex_t								g_mutex;
struct s_storage							*g_storage;

__attribute__((constructor)) static void	malloc_init(void)
{
	int		pagesize;

	pthread_mutex_init(&g_mutex, NULL);
	pthread_mutex_lock(&g_mutex);
	pagesize = getpagesize();



	pthread_mutex_unlock(&g_mutex);
}

__attribute__((destructor)) static void		malloc_destroy(void)
{
	struct s_region		*list;
	struct s_region		*next;

	pthread_mutex_lock(&g_mutex);
	list = g_storage->regions;
	while (list)
	{
		next = list->next;
		munmap(list->start, list->bytes_mapped);
		list = next;
	}

	pthread_mutex_unlock(&g_mutex);
	pthread_mutex_destroy(&g_mutex);
}


void		*malloc(size_t size)
{
	;
}
