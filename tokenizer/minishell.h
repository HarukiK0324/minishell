/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:23:34 by hkasamat          #+#    #+#             */
/*   Updated: 2025/05/19 16:19:23 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
# include <stdlib.h>
#include <unistd.h>

typedef enum
{
    TOKEN_PIPE,           // |
    TOKEN_REDIR_IN,       // <
    TOKEN_REDIR_OUT,      // >
    TOKEN_HEREDOC,        // <<
    TOKEN_APPEND,         // >>
    TOKEN_OPEN_PAREN,     // (
    TOKEN_CLOSE_PAREN,    // )
    TOKEN_SEMICOLON,      // ;
    TOKEN_AMPERSAND,      // &
    TOKEN_AND_IF,         // &&
    TOKEN_OR_IF,          // ||
    TOKEN_BACKTICK,       // `
    TOKEN_DOLLAR,         // $
    TOKEN_SINGLE_QUOTE,   // '
    TOKEN_DOUBLE_QUOTE,   // "
    TOKEN_BACKSLASH,      // '\'
    TOKEN_EQUALS,         // =
    TOKEN_WORD           // generic words
} TokenType;

typedef struct Token {
    TokenType type;
    char *value;
    Token *next;
} Token;

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