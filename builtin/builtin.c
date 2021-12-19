/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taejkim <taejkim@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/15 11:16:04 by kanlee            #+#    #+#             */
/*   Updated: 2021/12/19 17:16:52 by taejkim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "../minishell.h"
#include "../libft/libft.h"

int	is_builtin(char *cmd)
{
	return (ft_strequ(cmd, "echo") || ft_strequ(cmd, "cd")
		|| ft_strequ(cmd, "pwd") || ft_strequ(cmd, "export")
		|| ft_strequ(cmd, "unset") || ft_strequ(cmd, "env")
		|| ft_strequ(cmd, "exit"));
}

/* if caller is main process, caller should restore redirection
 * TODO: if caller is child process, should we? what about return value?
 * 		NOTE: execve is not called in this case
*/
int	exec_builtin(char **av)
{
	int	ac;

	ac = -1;
	while (av[++ac])
		;
	if (ft_strequ(av[0], "echo"))
		return (do_echo(ac, av));
	if (ft_strequ(av[0], "cd"))
		return (1);
	if (ft_strequ(av[0], "pwd"))
		return (1);
	if (ft_strequ(av[0], "export"))
		return (do_export(av, &g_env));
	if (ft_strequ(av[0], "unset"))
		return (do_unset(av, &g_env));
	if (ft_strequ(av[0], "env"))
		return (do_env(av, &g_env));
	if (ft_strequ(av[0], "exit"))
		return (do_exit(ac, av));
	return (0);
}

/* runs is main process.
 * apply redirection
 * execute builtin command
 * restore STDIN/STDOUT
*/
int	exec_builtin_single(char **av, t_rdinfo rd)
{
	int	stdin_bak;
	int	stdout_bak;
	int	ret;

	ret = 0;
	stdout_bak = dup(STDOUT_FILENO);
	dup2(rd.write, STDOUT_FILENO);
	stdin_bak = dup(STDIN_FILENO);
	dup2(rd.read, STDIN_FILENO);
	ret = exec_builtin(av);
	if (rd.write != STDOUT_FILENO)
	{
		close(rd.write);
		dup2(stdout_bak, STDOUT_FILENO);
		close(stdout_bak);
	}
	if (rd.read != STDIN_FILENO)
	{
		close(rd.read);
		dup2(stdin_bak, STDIN_FILENO);
		close(stdin_bak);
	}
	free(av);
	return (ret);
}
