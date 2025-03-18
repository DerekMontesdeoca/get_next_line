/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmontesd <dmontesd@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 13:07:39 by dmontesd          #+#    #+#             */
/*   Updated: 2025/03/18 02:38:20 by dmontesd         ###   ########.fr       */
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

	if (ensured <= str->size)
		return (true);
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

char	*get_next_line(int fd)
{
	static t_buf	buf;
	size_t			begin;
	t_dynstr		str;
	bool			nl_found;

	if (fd < 0)
		return (NULL);
	dynstr_init(&str);
	if (buf.bytes_read == 0 || buf.fd != fd)
	{
		buf.fd = fd;
		buf.index = 0;
		buf.bytes_read = 0;
	}
	nl_found = false;
	while (!nl_found)
	{
		if (buf.index >= (size_t) buf.bytes_read)
		{
			buf.bytes_read = read(buf.fd, buf.buf, BUFFER_SIZE);
			if (buf.bytes_read < 0)
				return (free(str.data), NULL);
			if (buf.bytes_read == 0)
				break;
			buf.index = 0;
		}
		begin = buf.index;
		while (buf.index < (size_t)buf.bytes_read && buf.buf[buf.index] != '\n')
			++buf.index;
		if (buf.index < (size_t) buf.bytes_read && buf.buf[buf.index] == '\n')
		{
			nl_found = true;
			++buf.index;
		}
		if (!dynstr_ensure_size(&str, str.len + (buf.index - begin) + 1))
			return (free(str.data), NULL);
		while (begin != buf.index)
			str.data[str.len++] = buf.buf[begin++];
		str.data[str.len] = '\0';
	}
	return (str.data);
}
