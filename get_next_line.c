/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 13:07:39 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/19 01:18:47 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "get_next_line.h"

bool	append_from_buf_to_line(t_buf *buf, t_dynstr *line)
{
	size_t	begin;

	begin = buf->index;
	while (buf->index < (size_t)buf->bytes_read && buf->buf[buf->index] != '\n')
		++buf->index;
	if (buf->index < (size_t) buf->bytes_read && buf->buf[buf->index] == '\n')
	{
		buf->nl_found = true;
		++buf->index;
	}
	if (!dynstr_ensure_size(line, line->len + (buf->index - begin) + 1))
		return (false);
	while (begin != buf->index)
		line->data[line->len++] = buf->buf[begin++];
	line->data[line->len] = '\0';
	return (true);
}

t_buf_read_result	fill_buffer_if_empty(t_buf *buf)
{
	if (buf->index >= (size_t) buf->bytes_read)
	{
		buf->bytes_read = read(buf->fd, buf->buf, BUFFER_SIZE);
		if (buf->bytes_read < 0)
			return (BUF_READ_RESULT_ERROR);
		if (buf->bytes_read == 0)
			return (BUF_READ_RESULT_DONE);
		buf->index = 0;
	}
	return (BUF_READ_RESULT_OK);
}

/**
 * The first call initializes the buffer. 
 * The other calls reset it to make it usable again if:
 * 1. A new FD is provided.
 * 2. The same FD is provided and the last call received the EOF.
 */
bool	buf_init(t_buf *buf, int fd)
{
	if (fd < 0)
		return (false);
	if (buf->buf == NULL)
	{
		if (BUFFER_SIZE <= 0)
			return (false);
		buf->buf = malloc(BUFFER_SIZE);
		if (buf->buf == NULL)
			return (false);
	}
	if (buf->bytes_read == 0 || buf->fd != fd)
	{
		buf->fd = fd;
		buf->index = 0;
		buf->bytes_read = 0;
	}
	return (true);
}

char	*get_next_line(int fd)
{
	static t_buf		buf;
	t_dynstr			line;
	t_buf_read_result	buf_read_result;

	if (!buf_init(&buf, fd))
		return (free(buf.buf), NULL);
	dynstr_init(&line);
	buf.nl_found = false;
	while (!buf.nl_found)
	{
		buf_read_result = fill_buffer_if_empty(&buf);
		if (buf_read_result == BUF_READ_RESULT_DONE)
		{
			free(buf.buf);
			buf.buf = NULL;
			break ;
		}
		if (buf_read_result == BUF_READ_RESULT_ERROR)
			return (free(line.data), NULL);
		if (!append_from_buf_to_line(&buf, &line))
			return (free(line.data), NULL);
	}
	return (line.data);
}
