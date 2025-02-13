/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 11:02:58 by dmontesd          #+#    #+#             */
/*   Updated: 2025/02/13 12:07:33 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stddef.h>
# include <stdbool.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 128
# endif

typedef struct s_buf
{
	int		fd;
	char	buf[BUFFER_SIZE];
	size_t	index;
	ssize_t	bytes_read;
	bool	nl_found;
}	t_buf;

typedef struct s_dynstr
{
	char	*data;
	size_t	size;
	size_t	len;
}	t_dynstr;

char	*get_next_line(int fd);

#endif
