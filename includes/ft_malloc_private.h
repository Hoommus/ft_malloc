/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_malloc_private.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 13:34:11 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/09 18:30:46 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef FT_MALLOC_PRIVATE_H
# define FT_MALLOC_PRIVATE_H

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

# define BLK_TINY_MAX 128
# define BLK_SMALL_MAX ((1 << 12))

# define BLK_MIN_SIZE 16

# define REGION_TINIES_SIZE ((BLK_TINY_MAX) * 128)
# define REGION_SMALLIES_SIZE (((size_t)BLK_SMALL_MAX) * 100)

# define PAGE_ADDRESS_MASK  0xFFFU // 12 bits of address space per page
# define PAGE_NUMBER_MASK (~0x0ULL) ^ (PAGE_ADDRESS_MASK)
# define ADDRESS_SPACE_HEADER 0xFFFF000000000000ULL

# define ZONE_MAGIC 0xBADBABE0DEADCAFE
# define REGION_MAGIC 0xBABEBAD0CAFEDEAD

enum								e_size_type
{
	BLK_TINY = 1,
	BLK_SMALL = 2,
	BLK_LARGE = 3
};

struct								s_block
{
	uint16_t			size;
	size_t				pointer:48;
} __attribute__((packed));

struct								s_zone
{
	uint64_t			zone_magic;
	enum e_size_type	type:3;
	bool				is_free:1;
	size_t				table_size_age:5;
	size_t				bytes_malloced:48;
	size_t				bytes_free;
	size_t				zone_size;
	size_t				table_size;
	size_t				table_bound;
	size_t				first_free_block_index;
	struct s_zone		*next;
	struct s_block		block_table[];
} __attribute__((aligned(8)));

struct								s_region
{
	uint64_t		magic;
	void			*start;
	bool			is_free:1;
	size_t			bytes_mapped;
	struct s_zone	*zones;
	struct s_zone	*large;
} __attribute__((aligned(8))); // 40 bytes

struct								s_stats
{
	struct rlimit	limits;
	size_t			total_allocated;
	size_t			total_mapped;
};

struct								s_storage
{
	void			*map_start;
	size_t			total_mapped;
	size_t			total_allocated;
	size_t			pagesize;
	size_t			threshold;
	struct s_stats	stats;
	size_t			regions_quantity;
	struct s_region	regions[];
} __attribute__((aligned(8)));

extern pthread_mutex_t				g_mutex;
extern struct s_storage *restrict	g_storage;

void								malloc_init(void);

bool								is_ptr_valid(void *ptr);
void								*alloc(size_t size, enum e_size_type type);
void								*alloc_largie(size_t size);
struct s_zone						*region_create_zone(struct s_region *region,
														 enum e_size_type type,
														 size_t size);
bool								in_region_bounds(struct s_region *region, void *ptr);
void								*get_block_straight(struct s_zone *zone, size_t size);
void								*get_block_reverse(struct s_zone *zone, size_t size);

void								zone_table_bounds(struct s_zone *zone, size_t pivot);

size_t								align_to(size_t size, size_t to_what);
void								print_hex_dump(void *ptr, size_t len, bool print_address);
void								print_hex_nbr(uint64_t n);
struct s_region						*region_create(struct s_region *region, void *start, size_t size);

#endif
