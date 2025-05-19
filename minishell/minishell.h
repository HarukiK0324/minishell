/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:23:34 by hkasamat          #+#    #+#             */
/*   Updated: 2025/04/30 11:04:57 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
# include <stdlib.h>

typedef struct s_cmd
{
    char	*cmd;
    char	**args;
}	t_cmd;

typedef struct s_prompt
{
    char *outfile;
    char *infile;
    char *errfile;
    t_cmd *cmds;
    int		cmd_count;
    char	*input;

}t_prompt;


#endif