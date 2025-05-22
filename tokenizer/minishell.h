/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:23:34 by hkasamat          #+#    #+#             */
/*   Updated: 2025/05/21 16:53:49 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

extern volatile sig_atomic_t g_signal;

typedef enum {
    TOKEN_PIPE,
    TOKEN_REDIR_IN,
    TOKEN_REDIR_OUT,
    TOKEN_HEREDOC,
    TOKEN_APPEND,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_SEMICOLON,
    TOKEN_AND_IF,
    TOKEN_OR_IF,
    TOKEN_DOLLAR,
    TOKEN_SINGLE_QUOTE,
    TOKEN_DOUBLE_QUOTE,
    TOKEN_WORD
} TokenType;

typedef enum {
    NODE_PIPE,
    NODE_CMD,
    NODE_SEMICOLON,
    NODE_AND_IF,
    NODE_OR_IF,
} NodeType;

typedef struct s_token {
    TokenType type;
    char *value;
    struct s_token *next;
} t_token;

typedef struct s_node {
    NodeType type;
    t_token *tokens;
    struct s_node *lhs;
    struct s_node *rhs;
} t_node;

#endif