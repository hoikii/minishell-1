/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanlee <kanlee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/11 17:02:02 by kanlee            #+#    #+#             */
/*   Updated: 2021/12/19 18:21:48 by kanlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "minishell.h"
#include "libft/libft.h"

static int	cnt_cmd_arg(t_cmd *node)
{
	int	n;

	n = 0;
	while (node != NULL)
	{
		if (node->cmd_type != TYPE_RDSIGN && node->cmd_type != TYPE_RDTARGET)
			n++;
		if (node->cmd_end)
			break ;
		node = node->next;
	}
	return (n);
}

// create NULL terminated array of strings based on given list.
// can be used by execv() family.
char	**listtostrarray(t_cmd *node)
{
	char	**str;
	int		n;
	int		i;

	n = cnt_cmd_arg(node);
	str = malloc(sizeof(char *) * (n + 1));
	if (!str)
	{
		perror("malloc failed");
		exit(1);
	}
	i = 0;
	while (node != NULL)
	{
		if (node->cmd_type != TYPE_RDSIGN && node->cmd_type != TYPE_RDTARGET)
			str[i++] = node->token;
		if (node->cmd_end)
			break ;
		node = node->next;
	}
	str[i] = NULL;
	return (str);
}

// node 이후에 pipe가 존재하는지 확인
// pipe 바로 전 또는 list_end까지가 하나의 커맨드
t_cmd	*has_pipe(t_cmd *node)
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

t_cmd	*has_heredoc(t_cmd *node)
{
	while (node != NULL)
	{
		if (node->next != NULL && ft_strequ(node->next->token, "<<"))
			return (node->next);
		node = node->next;
	}
	return (NULL);
}

void	safe_close_readend(int fd)
{
	if (fd != STDIN_FILENO)
		close(fd);
}
