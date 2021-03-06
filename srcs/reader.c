/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsousa <dsousa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/03/12 16:15:22 by dsousa            #+#    #+#             */
/*   Updated: 2014/03/27 17:51:18 by dsousa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <termcap.h>
#include <termios.h>
#include <unistd.h>
#include "sh.h"

void			freelist(t_line *list)
{
	t_line		*tmp;

	tmp = NULL;
	if (list)
		tmp = list->next;
	while ((list = tmp))
	{
		tmp = list->next;
		free(list);
	}
}

char			*create_line(t_line *list)
{
	int			len;
	char		*line;
	t_line		*tmp;

	tmp = list;
	if (!list->data)
	{
		line = ft_memalloc(sizeof(char *));
		line[0] = '\n';
		return (line);
	}
	len = list_len(list);
	line = ft_memalloc(sizeof(char) * (len + 1));
	len = 0;
	while (list)
	{
		line[len] = list->data;
		len++;
		list = list->next;
	}
	line[len] = '\0';
	freelist(tmp);
	return (line);
}

void			init_reader(int *cursor, t_line *list, t_ctrl_h *hist)
{
	*cursor = 0;
	list->data = 0;
	list->next = 0;
	hist->nb = -1;
	hist->unused = 0;
}

char			*reader(int fd, t_ctrl_h *hist)
{
	char		c[5];
	t_line		list;
	int			cursor;

	init_reader(&cursor, &list, hist);
	ft_bzero(c, 5);
	if (read(fd, c, 4) < 0)
		return (NULL);
	modif_list(&list, c, &cursor, hist);
	if (ft_match(c, '\n'))
		cursor = -42;
	ft_bzero(c, 5);
	while (cursor != -42 && read(fd, c, 4) > 0)
	{
		if (ft_match(c, '\n'))
		{
			write(1, "\n", 1);
			break ;
		}
		modif_list(&list, c, &cursor, hist);
		ft_bzero(c, 5);
	}
	return (create_line(&list));
}
