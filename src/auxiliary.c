/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   auxiliary.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 16:11:30 by vtarasiu          #+#    #+#             */
/*   Updated: 2020/03/07 20:04:02 by vtarasiu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_malloc_private.h"

bool	is_ptr_valid(void *ptr)
{
	if (((size_t)ptr) & ADDRESS_SPACE_HEADER)
		return (false);
	if ((size_t)ptr % 8 != 0)
		return (false);
	return (true);
}

size_t __attribute__((always_inline))
		align_to(size_t size, size_t to_what)
{
	return (size + ABS((to_what - size) % to_what));
}

void	print_hex_nbr(uint64_t n)
{
	static const char	alphabet[17] = "0123456789abcdef";
	char				line[2 + 16 + 1];
	size_t				i;

	ft_bzero(line, sizeof(line));
	line[0] = '0';
	line[1] = 'x';
	i = 17;
	while (i >= 2)
	{
		line[i--] = alphabet[n % 16];
		n /= 16;
	}
	ft_putstr(line);
}

void		print_hex_dump(void *ptr, size_t len, bool print_address)
{
	static const char	alphabet[17] = "0123456789abcdef";
	char				linebuf[64 + 32 + 1];
	size_t				i;
	size_t				j;

	ft_bzero(linebuf, sizeof(linebuf));
	i = -1;
	j = 0;
	while (++i < len)
	{
		linebuf[j++] = alphabet[(((uint8_t *)ptr)[i] >> 4) & 0xf];
		linebuf[j++] = alphabet[((uint8_t *)ptr)[i] & 0xf];
		linebuf[j++] = ' ';
		if (i && (i % 32 == 0 || i == len - 1))
		{
			linebuf[sizeof(linebuf) - 1] = 0;
			if (print_address)
				print_hex_nbr((uint64_t)(ptr + i - 32));
			ft_putchar(' ');
			ft_putendl(linebuf);
			ft_bzero(linebuf, sizeof(linebuf));
			j = 0;
		}
	}
}
