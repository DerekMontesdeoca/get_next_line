/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 13:07:39 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/17 21:05:16 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "get_next_line.h"

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	unsigned char	*old_ptr;
	unsigned char	*new_ptr;

	old_ptr = ptr;
	new_ptr = malloc(new_size);
	if (new_ptr == NULL)
		return (NULL);
	while (old_size--)
		new_ptr[old_size] = old_ptr[old_size];
	free(ptr);
	return (new_ptr);
}

void	dynstr_init(t_dynstr *str)
{
	str->len = 0;
	str->size = 0;
	str->data = NULL;
}

bool	dynstr_ensure_size(t_dynstr *str, size_t ensured)
{
	size_t	new_size;
	char	*new_data;

	new_size = str->size;
	while (new_size < ensured)
	{
		if (new_size == 0)
			new_size = 128;
		else
			new_size *= 2;
	}
	new_data = ft_realloc(str->data, str->size, new_size);
	if (new_data == NULL)
	{
		free(str->data);
		str->data = NULL;
		return (false);
	}
	str->data = new_data;
	str->size = new_size;
	return (true);
}

static size_t	buf_nl(t_buf *buf, int fd)
{
	size_t	prev_start;

	if (fd != buf->fd || buf->bytes_read <= 0)
	{
		buf->bytes_read = 0;
		buf->index = 0;
		buf->fd = fd;
	}
	if (buf->index >= (size_t)buf->bytes_read)
	{
		buf->bytes_read = read(fd, buf->buf, BUFFER_SIZE);
		if (buf->bytes_read <= 0)
			return (0);
		buf->index = 0;
	}
	prev_start = buf->index;
	while (buf->index < (size_t)buf->bytes_read && buf->buf[buf->index] != '\n')
		++buf->index;
	buf->nl_found = buf->index != (size_t)buf->bytes_read;
	if (buf->nl_found)
		++buf->index;
	return (prev_start);
}

char	*get_next_line(int fd)
{
	static t_buf	buf;
	size_t			prev_start;
	t_dynstr		str;

	if (fd < 0)
		return (NULL);
	dynstr_init(&str);
	buf.nl_found = false;
	while (!buf.nl_found)
	{
		prev_start = buf_nl(&buf, fd);
		if (buf.bytes_read < 0)
			return (free(str.data), NULL);
		if (buf.bytes_read == 0)
			break ;
		if (buf.index - prev_start + str.len + 1 > str.size)
			if (!dynstr_ensure_size(&str, buf.index - prev_start + str.len + 1))
				return (NULL);
		while (prev_start < buf.index)
			str.data[str.len++] = buf.buf[prev_start++];
	}
	if (str.size == 0)
		return (NULL);
	str.data[str.len] = '\0';
	return (str.data);
}
