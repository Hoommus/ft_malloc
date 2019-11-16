/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <vtarasiu@student.unit.ua>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:01:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/08/06 21:47:14 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <sys/mman.h>
# include <sys/types.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <unistd.h>
# include <pthread.h>
# include "libft.h"

# define ALIGNMENT sizeof(long);

# define BLOCK_TINY_SIZE 64
# define BLOCK_SMALL_SIZE 1024

enum			e_size_type
{
	BLK_TINY = 1,
	BLK_SMALL = 2,
	BLK_LARGE = 3
};

struct			s_region
{
	void		*start;
	struct			s_zone
	{
		enum e_size_type	type;
		void				*start;
		struct s_block		*free_blocks;
	}				tiny;
	struct s_zone	small;
	struct s_zone	large;
	size_t			bytes_malloced;
	size_t			bytes_mapped;

	struct s_region	*next;
};

struct			s_block
{
	enum e_size_type	type : 2;
	bool				is_free : 1;
	uint64_t			size : 48;
	void				*next;
} __attribute__((packed));

struct			s_storage
{
	void			*map_start;
	int				pagesize;
	rlim_t			rlim_cur;
	rlim_t			rlim_max;
	struct s_region	*regions;

};

extern pthread_mutex_t		g_mutex;
extern struct s_storage		*g_storage;

void						ft_free(void *ptr);
void						*ft_malloc(size_t size);
void						*ft_realloc(void *ptr, size_t size);

#endif
