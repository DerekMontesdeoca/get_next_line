/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 13:07:39 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/19 02:38:44 by dmontesd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "get_next_line_bonus.h"

static bool	append_from_buf_to_line(t_buf *buf, t_dynstr *line)
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

static t_buf_read_result	fill_buffer_if_empty(t_buf *buf, int fd)
{
	if (buf->index >= (size_t) buf->bytes_read)
	{
		buf->bytes_read = read(fd, buf->buf, BUFFER_SIZE);
		if (buf->bytes_read < 0)
			return (BUF_READ_RESULT_ERROR);
		if (buf->bytes_read == 0)
			return (BUF_READ_RESULT_DONE);
		buf->index = 0;
	}
	return (BUF_READ_RESULT_OK);
}

static bool	buf_init(t_buf *buf)
{
	if (buf->buf == NULL)
	{
		buf->buf = malloc(BUFFER_SIZE);
		if (buf->buf == NULL)
			return (false);
	}
	if (buf->bytes_read == 0)
		buf->index = 0;
	return (true);
}

char	*get_next_line(int fd)
{
	static t_buf		bufs[1024];
	t_dynstr			line;
	t_buf_read_result	buf_read_result;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	if (!buf_init(&bufs[fd]))
		return (NULL);
	dynstr_init(&line);
	bufs[fd].nl_found = false;
	while (!bufs[fd].nl_found)
	{
		buf_read_result = fill_buffer_if_empty(&bufs[fd], fd);
		if (buf_read_result == BUF_READ_RESULT_DONE)
		{
			free(bufs[fd].buf);
			bufs[fd].buf = NULL;
			break ;
		}
		if (buf_read_result == BUF_READ_RESULT_ERROR)
			return (free(line.data), NULL);
		if (!append_from_buf_to_line(&bufs[fd], &line))
			return (free(line.data), NULL);
	}
	return (line.data);
}
