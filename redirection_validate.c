/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_validate.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanlee <kanlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/18 17:19:52 by kanlee            #+#    #+#             */
/*   Updated: 2021/12/18 22:53:15 by kanlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "minishell.h"
#include "libft/libft.h"
#include "parse/tmp_listfunc.h"

static t_cmd	*has_pipe(t_cmd *node)
{
	while (node != NULL)
	{
		if (node->next == NULL || ft_strequ(node->next->token, "|"))
		{
			node->cmd_end = 1;
			return (node->next);
		}
		node = node->next;
	}
	return (NULL);
}

int	fileexists(char *target)
{
	struct stat	buf;

	if (stat(target, &buf) < 0 && errno == ENOENT)
		return (0);
	return (1);
}

int	is_in_list(char *str, t_list *list)
{
	while (list != NULL)
	{
		if (ft_strequ(str, (char *)(list->content)))
			return (1);
		list = list->next;
	}
	return (0);
}

int	is_target_valid(t_cmd *node, t_list **willcreate)
{
	int		fd;
	char	*rdtarget;

	rdtarget = node->next->token;
	if (is_redirection_node(node) == RD_READ)
	{
		if (is_in_list(rdtarget, *willcreate))
			return (1);
		fd = open(rdtarget, O_RDONLY);
		if (fd < 0)
		{
			perror(rdtarget);
			return (0);
		}
		close(fd);
	}
	if (is_redirection_node(node) == RD_WRITE
		|| is_redirection_node(node) == RD_APPEND)
	{
		if (!fileexists(rdtarget))
			ft_lstadd_back(willcreate, ft_lstnew(ft_strdup(rdtarget)));
	}
	return (1);
}

void	validate_redirection(t_cmd *node)
{
	int		rdtype;
	t_cmd	*head;
	t_list	*willcreate;

	head = node;
	willcreate = 0;
	while (!node->cmd_end)
	{
		rdtype = is_redirection_node(node);
		if (rdtype == RD_READ || rdtype == RD_WRITE || rdtype == RD_APPEND)
		{
			if (!is_target_valid(node, &willcreate))
			{
				head->cmd_type = TYPE_INVALID;
				break ;
			}
		}
		node = node->next;
	}
	ft_lstclear(&willcreate, free);
}

void	chk_rdtarget(t_cmd *node)
{
	t_cmd	*pipenode;

	while (1)
	{
		pipenode = has_pipe(node);
		if (!pipenode)
			break ;
		validate_redirection(node);
		node = pipenode->next;
	}
	validate_redirection(node);
}
