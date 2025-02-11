/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 13:07:39 by dmontesd          #+#    #+#             */
/*   Updated: 2025/02/11 17:57:54 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#ifndef BUFFER_SIZE
 #define BUFFER_SIZE 50
#endif

typedef struct s_buf
{
	unsigned char	buf[BUFFER_SIZE];
	unsigned char	*start;
	unsigned char	*line_end;
	unsigned char	*end;
	ssize_t			bytes_read;
}	t_buf;

ssize_t	buf_nl(t_buf *buf, int fd)
{
	ssize_t			bytes_read;
	unsigned char	*ptr;

	if (buf->line_end == NULL)
	{
		buf->bytes_read = read(fd, buf->buf, BUFFER_SIZE);
		if (bytes_read <= 0)
			return (buf->bytes_read);
		buf->end = buf->buf + buf->bytes_read;
		ptr = buf->buf;
	}
	else 
		ptr = buf->line_end;
	while (ptr != buf->end && *ptr != '\n')
		++ptr;
	if (ptr != buf->end)
		buf->line_end = ptr + 1;
	return (1);
}

char	*get_next_line(int fd)
{
	static t_buf	buf;
	char			*str;
	size_t			str_size;
	unsigned char	*nl;

	str = NULL;
	nl = NULL;
	str_size = 0;
	while (buf.end != buf.buf && nl == NULL)
	{
		realloc(str, str_size + buf.end - buf.buf);
		if (buf_nl(&buf, fd) <= 0)
			if (buf.bytes_read == 0)
		nl = buf.start;
	}
}
