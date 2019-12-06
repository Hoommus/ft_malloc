/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 15:01:02 by vtarasiu          #+#    #+#             */
/*   Updated: 2019/12/01 17:57:26 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <stdint.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <stdbool.h>
# include <stddef.h>
# include <unistd.h>
# include <pthread.h>
# include "libft.h"
# ifdef __linux__
#  include <sys/resource.h>
# endif

# define ALIGNMENT sizeof(size_t)

# define ALIGN_TO_PAGE(x, pgsz) ((x) + (x) % (pgsz))
# define ALIGN_TO_ARCH(x)       ((x) + (x) % ALIGNMENT)

# define BLOCK_TINY_SIZE 128
# define BLOCK_SMALL_SIZE 2048

# define BLOCK_MIN_SIZE 16

# define REGION_TINIES_SIZE (BLOCK_TINY_SIZE * 128)
# define REGION_SMALLIES_SIZE (BLOCK_SMALL_SIZE * 128)
# define MIN_RESERVED ALIGN_TO_PAGE(REGION_TINIES_SIZE + REGION_SMALLIES_SIZE)

# define PAGE_ADDRESS_MASK  0xFFFU // 12 bits of address space per page
# define PAGE_NUMBER_MASK (~0x0ULL) ^ (PAGE_ADDRESS_MASK)

enum			e_size_type
{
	BLK_TINY = 1,
	BLK_SMALL = 2,
	BLK_LARGE = 3
};

struct 			s_tblk
{
	enum e_size_type	type:1;
	uint8_t 			size:7;
	ptrdiff_t			pointer:48;
} __attribute__((packed));

struct			s_sblk
{
	enum e_size_type	type:2;
	uint16_t			size:11;
	ptrdiff_t			pointer:48;
};

// Zones contain blocks that will be returned via a malloc() call
struct			s_zone
{
	enum e_size_type	type:3;
	bool				is_full:1;
	bool				is_free:1;
	struct s_zone		*next;
	uint16_t			table_size;
	struct s_block		*blocks;
};

// Regions contain continuous memory that was acquired from every mmap() call
#if __POINTER_WIDTH__ == 64

struct			s_region
{
	void			*start;
	bool			is_full:1;
	bool			is_free:1;
	size_t			bytes_malloced:48;
	size_t			bytes_mapped:48;
	struct s_zone	*tinies;
	struct s_zone	*smallies;
	struct s_zone	*largies;
	struct s_region	*next;
} __attribute__((aligned(8)));

struct			s_block
{
	enum e_size_type	type : 2;
	bool				is_free : 1;
	size_t				size : 48;
} __attribute__((packed, aligned(8)));

#elif __POINTER_WIDTH__ == 32

struct			s_region
{
	void			*start;
	bool			is_full:1;
	bool			is_free:1;
	size_t			bytes_malloced;
	size_t			bytes_mapped;
	struct s_zone	*tinies;
	struct s_zone	*smallies;
	struct s_zone	*largies;
	struct s_region	*next;
} __attribute__((aligned(4)));

struct			s_block
{
	enum e_size_type	type : 2;
	bool				is_free : 1;
	size_t				size : 32;
} __attribute__((packed, aligned(4)));

#else
#error "unsupported arch"
#endif

struct			s_stats
{

};

struct			s_storage
{
	void			*map_start;
	size_t			total_mapped;
	size_t			total_allocated;
	size_t			pagesize;
	rlim_t			rlim_cur;
	rlim_t			rlim_max;
	struct s_stats	stats;
	size_t 			regions_quantity;
	struct s_region	*regions;
} __attribute__((aligned(8)));

extern pthread_mutex_t				g_mutex;
extern struct s_storage *restrict	g_storage;

void						ft_free(void *ptr);
void						*ft_malloc(size_t size);
void						*ft_realloc(void *ptr, size_t size);

#endif
