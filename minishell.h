/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: taejkim <taejkim@student.42seoul.kr>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/11 17:00:27 by kanlee            #+#    #+#             */
/*   Updated: 2021/12/19 17:12:28 by taejkim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include "parse/tmp_listfunc.h"

extern int	g_exit_status;
extern t_env *g_env;

typedef struct s_pipeinfo {
	int	read;
	int	write;
	int	unused;
}	t_pipeinfo;

typedef struct s_rdinfo {
	int	read;
	int	write;
}	t_rdinfo;

enum e_rdtype {
	NONE = 0,
	RD_WRITE,
	RD_APPEND,
	RD_READ,
	RD_HEREDOC
};

void	sig_handler(int signo);

/* parser */
void	get_line(char **line);
void	parse(t_cmd **ptr, char *line, int *err_flag);
int		check_cmd(t_cmd *cmd);

/* exec_line.c */
int		exec_line(t_cmd *head);

/* exec_command.c */
int		command(t_cmd *node, t_pipeinfo pipeinfo);

/* ft_execvpe.c */
int		ft_execvpe(char *cmd, char **arg, char **env);

/* redirection.c */
void	chk_rdtarget(t_cmd *node);
int		is_redirection_node(t_cmd *node);
int		store_rdinfo(t_cmd *node, t_rdinfo *rd, int rdtype);
int		read_heredoc(t_cmd *node, char *limit);

/* utils.c */
void	ft_lstcut(t_list *lst, t_list *el);
void	ft_lstremove(t_list **head, t_list *el);
char	**listtostrarray(t_cmd *list);
void	safe_close_readend(int fd);


// >>>>
/* error.c */
void	error_out(char *str);
void	err_print(int err_flag);
void	errno_print(int errnum);
void	identifier_err_print(void);

/* builtin/builtin.c */
int		is_builtin(char *cmd);
int		exec_builtin_single(char **av, t_rdinfo rd);
int		exec_builtin(char **av);
int		do_echo(int ac, char **av);
int		do_exit(int ac, char **av);

// >>>>
int		do_env(char **av, t_env **env);
int		do_export(char **av, t_env **env);
int		do_unset(char **av, t_env **env);

#endif
