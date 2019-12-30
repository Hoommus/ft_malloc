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

# include <stdint.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <stdbool.h>
# include <stddef.h>
# include <unistd.h>
# include <pthread.h>
# include <limits.h>
# include "libft.h"
# ifdef __linux__
#  include <sys/resource.h>
# endif

# define ALIGNMENT sizeof(size_t)

# ifndef ABS
#  define ABS(x) ((x) < 0 ? -(x) : (x))
# endif

# define ALIGN_TO_PAGE(x, pgsz) ((x) + ABS((pgsz - x) % (pgsz)))
# define ALIGN_TO_ARCH(x)       ((x) + ABS((ALIGNMENT - x) % ALIGNMENT))

# define BLK_TINY_MAX 128
# define BLK_SMALL_MAX g_storage->pagesize

# define BLOCK_MIN_SIZE 16

# define REGION_TINIES_SIZE ((BLK_TINY_MAX) * 128)
# define REGION_SMALLIES_SIZE (((size_t)BLK_SMALL_MAX) * 128)

# define PAGE_ADDRESS_MASK  0xFFFU // 12 bits of address space per page
# define PAGE_NUMBER_MASK (~0x0ULL) ^ (PAGE_ADDRESS_MASK)
# define ADDRESS_SPACE_HEADER 0xFFFF000000000000ULL

# define ZONE_MAGIC 0xBADBABE0DEADCAFE

enum								e_size_type
{
	BLK_TINY = 1,
	BLK_SMALL = 2,
	BLK_LARGE = 3
};

struct								s_block
{
	bool				is_free:1;
	uint16_t			size:12;
	size_t				pointer : 64; // TODO: 3 bits left
} __attribute__((packed,aligned(8)));

// Zones contain blocks that will be returned via a malloc() call
struct								s_zone
{
	uint64_t			zone_magic;
	enum e_size_type	type:3;
	bool				is_full:1;
	bool				is_free:1;
	size_t				bytes_malloced:48;
	size_t				zone_size;
	size_t				table_size:16;
	size_t				first_free_block_index:16;
	struct s_zone		*next;
	struct s_block		block_table[]; // TODO: restrict size to page boundary
} __attribute__((aligned(8)));

// Regions contain continuous memory that was acquired from every mmap() call
# if __POINTER_WIDTH__ == 64

struct								s_region
{
	void			*start;
	bool			is_full:1;
	size_t			bytes_malloced:48;
	size_t			bytes_mapped:48;
	struct s_zone	*zones;
	struct s_zone	*large;
} __attribute__((aligned(8))); // 40 bytes

# elif __POINTER_WIDTH__ == 32

struct								s_region
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

struct								s_block
{
	enum e_size_type	type : 2;
	bool				is_free : 1;
	size_t				size : 32;
} __attribute__((packed,aligned(4)));

# else
#  error "unsupported arch"
# endif

struct								s_stats
{
	struct rlimit	limits;
	size_t			total_allocated;
	size_t			total_mapped;
	int				dummy;
};

struct								s_storage
{
	void			*map_start;
	size_t			total_mapped;
	size_t			total_allocated;
	size_t			pagesize;
	struct s_stats	stats;
	size_t			regions_quantity;
	struct s_region	regions[];
} __attribute__((aligned(8)));

extern pthread_mutex_t				g_mutex;
extern struct s_storage *restrict	g_storage;

bool								ptr_seems_valid(void *ptr);
void								*alloc(size_t size, enum e_size_type type);
void								*alloc_largie(size_t size);
struct s_zone						*region_create_zone(struct s_region *region,
														enum e_size_type type,
														size_t size);
void								show_alloc_mem(void);
bool								in_region_bounds(struct s_region *region, void *ptr);

#endif
