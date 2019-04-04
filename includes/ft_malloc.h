/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:01:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/04/04 22:31:12 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <sys/mman.h>
# include <sys/types.h>
# include <stdbool.h>
# include <unistd.h>
# include "libft.h"

# define ALIGNMENT sizeof(long);

enum			e_size_type
{
	BLK_TINY = 1,
	BLK_SMALL = 2,
	BLK_LARGE = 3
};

struct			s_zone
{
	enum e_size_type	type;
	struct s_block		*blocks;
};

struct			s_block
{
	enum e_size_type	type : 2;
	bool				is_free : 1;
	struct s_block		*next;
} __attribute__((packed));

extern pthread_mutex_t		g_mutex;

void						ft_free(void *ptr);
void						*ft_malloc(size_t size);
void						*ft_realloc(void *ptr, size_t size);

#endif
