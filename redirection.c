/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taejkim <taejkim@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/12 20:53:33 by kanlee            #+#    #+#             */
/*   Updated: 2021/12/27 18:04:59 by taejkim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "minishell.h"
#include "libft/libft.h"

static int	open_target(int rdtype, char *rdtarget)
{
	int	fd;

	if (rdtype == RD_WRITE)
		fd = open(rdtarget, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (rdtype == RD_APPEND)
		fd = open(rdtarget, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (rdtype == RD_READ)
		fd = open(rdtarget, O_RDONLY);
	if (rdtype == RD_HEREDOC)
		fd = ft_atoi(rdtarget);
	return (fd);
}

int	store_rdinfo(t_cmd *node, t_rdinfo *rd, int rdtype)
{
	char	*rdtarget;
	int		fd;

	rdtarget = node->next->token;
	fd = open_target(rdtype, rdtarget);
	if (fd < 0)
	{
		errno_print(errno, rdtarget);
		return (-1);
	}
	if (rdtype == RD_WRITE || rdtype == RD_APPEND)
	{
		// if write redirection is already set, ignore former
		if (rd->write != STDOUT_FILENO)
			close(rd->write);
		rd->write = fd;
	}
	if (rdtype == RD_READ || rdtype == RD_HEREDOC)
	{
		// if read redirection is already set, ignore former
		if (rd->read != STDIN_FILENO)
			close(rd->read);
		rd->read = fd;
	}
	return (0);
}
