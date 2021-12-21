/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taejkim <taejkim@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/17 17:17:27 by taejkim           #+#    #+#             */
/*   Updated: 2021/12/19 17:20:16 by kanlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "../minishell.h"

static void	print_export(t_env *env)
{
	while (env)
	{
		if (env->is_env)
			printf("declare -x %s=\"%s\"\n", env->key, env->value);
		else
			printf("declare -x %s\n", env->key);
		env = env->next;
	}
}

static int	is_valid_export_str(char *str)
{
	int	start_not_number;
	int	i;

	start_not_number = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == '=' && i > 0)
			return (1);
		if (!is_allow_envpname(str[i]))
			return (0);
		if (!ft_isdigit(str[i]))
			start_not_number = 1;
		else if (!start_not_number)
			return (0);
		++i;
	}
	return (1);
}

static char	*has_equal_sign(char *str)
{
	while (*str)
	{
		if (*str == '=')
			return (str);
		++str;
	}
	return (0);
}

static int	export_str(char *str, t_env **env)
{
	char	*equal;

	if (!is_valid_export_str(str))
		return (-1);
	equal = has_equal_sign(str);
	if (equal)
	{
		*equal = 0;
		if (has_env(str, *env))
			modify_env(str, equal + 1, *env);
		else
			add_env(str, equal + 1, env, 1);
		*equal = '=';
	}
	else
	{
		if (!has_env(str, *env))
			add_env(str, "", env, 0);
	}
	return (0);
}

int	do_export(char **av, t_env **env)
{
	int	error_flag;

	error_flag = 0;
	++av;
	if (!*av)
	{
		print_export(*env);
		return (0);
	}
	while (*av)
	{
		error_flag = export_str(*av, env);
		++av;
	}
	if (error_flag)
	{
		identifier_err_print();
		return (1);
	}
	return (0);
}
