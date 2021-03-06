/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smakroum <smakroum@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2014/03/14 18:46:49 by rbenjami          #+#    #+#             */
/*   Updated: 2014/03/27 17:09:11 by smakroum         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "sh.h"

void		op_pipe3(t_ast *tree, t_token *token, int pfd[2], int *pid)
{
	if (token)
		fill_tree(token, &tree->right);
	if (tree->right && tree->right->tk->prio > 0)
		resolve_tree(tree->right, pfd);
	else
		op_pipe2(tree, pfd, pid);
}

void		op_pipe2(t_ast *tree, int pfd[2], int *pid)
{
	int			status;
	char		*line;
	t_token		*token;

	token = NULL;
	if (tree->right)
		*pid = execute(tree->right->tk->value, NULL, pfd, 0);
	else
	{
		waitpid(*pid, &status, 0);
		g_handler.cmd = WEXITSTATUS(status);
		g_handler.len = 6;
		turn_on(g_handler.term);
		ft_putstr("\033[31mpipe> \033[m");
		line = reader(0, g_handler.hist);
		lexer(&token, line);
		parse_string(&token);
		ft_strdel(&line);
		g_handler.len = 0;
		if (ft_modify_token_for_redir(&token) != -42)
			op_pipe3(tree, token, pfd, pid);
	}
	close_pfd(pfd);
}

void		ft_redir_pipe(t_ast *tree, int pfd_old[2], int pfd[2])
{
	int			pid;
	int			status;

	pid = op_redir2(tree->left, pfd_old, pfd);
	waitpid(pid, &status, 0);
	g_handler.cmd = WEXITSTATUS(status);
}

pid_t		op_pipe(t_ast *tree, int pfd_old[2])
{
	int			pfd[2];
	pid_t		pid;

	pid = 0;
	if (pipe(pfd) != -1)
	{
		if (tree->left)
		{
			if (tree->left->tk->prio == 1)
				ft_redir_pipe(tree, pfd_old, pfd);
			else
				pid = execute(tree->left->tk->value, pfd_old, pfd, 1);
			if (pfd_old)
				close_pfd(pfd_old);
			if (tree->right && tree->right->tk->prio > 0)
				resolve_tree(tree->right, pfd);
			else
				op_pipe2(tree, pfd, &pid);
		}
		else
			error("42sh: parse error near `|'\n");
	}
	return (pid);
}
