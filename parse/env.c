/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taejkim <taejkim@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/19 20:07:55 by taejkim           #+#    #+#             */
/*   Updated: 2021/12/19 16:22:33 by kanlee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../minishell.h"

char	*get_value(t_env *env, char *key)
{
	while (env)
	{
		if (ft_strequ(key, env->key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

int	has_env(char *key, t_env *env)
{
	while (env)
	{
		if (ft_strequ(key, env->key))
			return (1);
		env = env->next;
	}
	return (0);
}

void	add_env(char *key, char *value, t_env **env, int is_env)
{
	t_env	*head;
	t_env	*node;

	node = (t_env *)malloc(sizeof(t_env));
	if (!node)
		error_out(" malloc error");
	node->is_env = is_env;
	node->key = ft_strdup(key);
	node->value = ft_strdup(value);
	node->next = NULL;
	head = *env;
	if (head == NULL)
		*env = node;
	else
	{
		while (head->next)
			head = head->next;
		head->next = node;
	}
}

void	modify_env(char *key, char *value, t_env *env)
{
	char	*tmp;

	while (env)
	{
		if (ft_strequ(key, env->key))
		{
			tmp = env->value;
			env->value = ft_strdup(value);
			env->is_env = 1;
			free(tmp);
			return ;
		}
		env = env->next;
	}
}

t_env	*make_env(char **envp, char *key, char *value)
{
	t_env	*head;
	t_env	*tmp;

	head = NULL;
	while (*envp)
	{
		key = *envp;
		while (**envp && **envp != '=')
			++(*envp);
		if (**envp)
			value = *envp + 1;
		else
			value = *envp;
		**envp = 0;
		add_env(key, value, &head, 1);
		++envp;
	}
	return (head);
}
