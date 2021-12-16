/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tmp_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taejkim <taejkim@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/08 16:59:27 by taejkim           #+#    #+#             */
/*   Updated: 2021/12/17 03:57:41 by taejkim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <termios.h>
#include <term.h>
#include "tmp_listfunc.h"
#include "../libft/libft.h"
#include "../minishell.h"

char	*append(char *str, char c)
{
	char	*res;
	int	len;
	int	i;

	len = ft_strlen(str) + 1;
	res = (char *)malloc(sizeof(char) * (len + 1));
	i = 0;
	while (i < len - 1)
	{
		res[i] = str[i];
		++i;
	}
	res[i++] = c;
	res[i] = 0;
	free(str);
	return (res);
}

void	get_line(char **line)
{
	extern int rl_catch_signals;

	rl_catch_signals = 0;
	if (*line)
	{
		free(*line);
		*line = NULL;
	}
	*line = readline("$>");
	if (*line == NULL)
		error_out("exit");
	if (ft_strncmp(*line, "", 1))
		add_history(*line);
}

int	is_space(char c)
{
	if (c == ' ' || c == '\t')
		return (1);
	return (0);
}

int	is_separator(char c)
{
	if (c == '|' || c == '>' || c == '<')
		return (1);
	return (0);
}

int	is_allow_envpname(char c)
{
	if ('0' <= c && c <= '9')
		return (1);
	if ('a' <= c && c <= 'z')
		return (1);
	if ('A' <= c && c <= 'Z')
		return (1);
	if (c == '_')
		return (1);
	return (0);
}

int	is_unspecified_char(char c)
{
	if (c == '\\' || c == ';' || c == '(' \
		|| c == ')' || c == '*' || c == '&' || c == '#')
		return (1);
	return (0);
}

char	*separate_pipe(t_cmd **ptr, t_cmd *cmd, char *line)
{
	t_cmd	*tmp;

	if (!ft_strncmp(cmd->token, "", 1))
	{
		cmd->token = append(cmd->token, '|');
		add_cmd(ptr, cmd);
	}
	else
	{
		add_cmd(ptr, cmd);
		tmp = init_cmd();
		tmp->token = append(tmp->token, '|');
		add_cmd(ptr, tmp);
	}
	return (line + 1);
}

char	*separate_rdr_out(t_cmd **ptr, t_cmd *cmd, char *line)
{
	t_cmd	*tmp;

	if (!ft_strncmp(cmd->token, "", 1))
	{
		cmd->token = append(cmd->token, '>');
		if (!ft_strncmp(line, ">>", 2))
			cmd->token = append(cmd->token, '>');
		add_cmd(ptr, cmd);
	}
	else
	{
		add_cmd(ptr, cmd);
		tmp = init_cmd();
		tmp->token = append(tmp->token, '>');
		if (!ft_strncmp(line, ">>", 2))
			tmp->token = append(tmp->token, '>');
		add_cmd(ptr, tmp);
	}
	if (!ft_strncmp(line, ">>", 2))
		return (line + 2);
	return (line + 1);
}

char	*separate_rdr_in(t_cmd **ptr, t_cmd *cmd, char *line)
{
	t_cmd	*tmp;

	if (!ft_strncmp(cmd->token, "", 1))
	{
		cmd->token = append(cmd->token, '<');
		if (!ft_strncmp(line, "<<", 2))
			cmd->token = append(cmd->token, '<');
		add_cmd(ptr, cmd);
	}
	else
	{
		add_cmd(ptr, cmd);
		tmp = init_cmd();
		tmp->token = append(tmp->token, '<');
		if (!ft_strncmp(line, "<<", 2))
			tmp->token = append(tmp->token, '<');
		add_cmd(ptr, tmp);
	}
	if (!ft_strncmp(line, "<<", 2))
		return (line + 2);
	return (line + 1);
}

char	*separate(t_cmd **ptr, t_cmd **cmd, char *line)
{
	if (is_space(*line))
		add_cmd(ptr, *cmd);
	else if (*line == '|')
		line = separate_pipe(ptr, *cmd, line);
	else if (*line == '>')
		line = separate_rdr_out(ptr, *cmd, line);
	else if (*line == '<')
		line = separate_rdr_in(ptr, *cmd, line);
	while (*line && is_space(*line))
		++line;
	if (*line)
		*cmd = init_cmd();
	else
		*cmd = NULL;
	return (line);
}

char	*envp_decision(t_cmd *cmd, char *line)
{
	t_env_key	*env_key;
	
	env_key = init_env_key();
	++line;
	if (!(*line) || *line == ' ' || *line == '\"')
	{
		env_key->key = append(env_key->key, '$');
		add_env_key(cmd, env_key);
	}
	else if (*line == '\'')
		add_env_key(cmd, env_key);
	else
	{
		env_key->is_key = 1;
		while (*line && is_allow_envpname(*line))
			env_key->key = append(env_key->key, *(line++));
		add_env_key(cmd, env_key);
	}
	cmd->token = append(cmd->token, '$');
	return (line);
}

char	*small_quote_envp_decision(t_cmd *cmd, char *line)
{
	t_env_key	*env_key;
	
	env_key = init_env_key();
	env_key->key = append(env_key->key, '$');
	add_env_key(cmd, env_key);
	cmd->token = append(cmd->token, '$');
	return (line + 1);
}

char	*big_quote_envp_decision(t_cmd *cmd, char *line)
{
	t_env_key	*env_key;
	
	env_key = init_env_key();
	++line;
	if (!(*line) || *line == ' ' || *line == '\'' || *line == '\"')
	{
		env_key->key = append(env_key->key, '$');
		add_env_key(cmd, env_key);
	}
	else
	{
		env_key->is_key = 1;
		while (*line && is_allow_envpname(*line))
			env_key->key = append(env_key->key, *(line++));
		add_env_key(cmd, env_key);
	}
	cmd->token = append(cmd->token, '$');
	return (line);
}

char	*start_quote(t_cmd *cmd, char *line, int *err_flag)
{
	char quote;

	quote = *line;
	++line;
	while (*line)
	{
		if (*line == quote)
			return (line + 1);
		if (*line == '$')
		{
			if (quote == '\"')
				line = big_quote_envp_decision(cmd, line);
			else
				line = small_quote_envp_decision(cmd, line);
		}
		else
			cmd->token = append(cmd->token, *(line++));
	}
	*err_flag = QUOTE_ERR;
	return (line);
}

void	parse(t_cmd **ptr, char *line, int *err_flag)
{
	t_cmd	*cmd;
	
	if (*ptr)
		destroy_cmd(ptr);
	if (!(*line))
		*err_flag = EMPTY_LINE;
	cmd = init_cmd();
	while (*line && is_space(*line))
		++line;
	while (*line)
	{
		if (is_unspecified_char(*line))
			*err_flag = SYNTAX_ERR;
		if (is_space(*line) || is_separator(*line))
			line = separate(ptr, &cmd, line);
		else if (*line == '\"' || *line == '\'')
			line = start_quote(cmd, line, err_flag);
		else if (*line == '$')
			line = envp_decision(cmd, line);
		else
			cmd->token = append(cmd->token, *(line++));
	}
	add_cmd(ptr, cmd);
}

int	check_cmd(t_cmd *cmd)
{
	int	prev_token_is_pipe;
	int	prev_token_is_rdr;

	if (ft_strequ(cmd->token, "|"))
		return (SYNTAX_ERR);
	prev_token_is_pipe = 0;
	prev_token_is_rdr = 0;
	while (cmd)
	{
		if ((prev_token_is_pipe || prev_token_is_rdr) && ft_strequ(cmd->token, "|"))
			return (SYNTAX_ERR);
		if (ft_strequ(cmd->token, "|"))
			prev_token_is_pipe = 1;
		else
			prev_token_is_pipe = 0;
		if (ft_strequ(cmd->token, "<") || ft_strequ(cmd->token, ">") ||\
				ft_strequ(cmd->token, "<<") || ft_strequ(cmd->token, ">>"))
			prev_token_is_rdr = 1;
		else
			prev_token_is_rdr = 0;
		if (!(cmd->next) && ft_strequ(cmd->token, "|"))
			return (SYNTAX_ERR);
		cmd = cmd->next;
	}
	return (0);
}

