/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 14:14:07 by hkasamat          #+#    #+#             */
/*   Updated: 2025/03/29 21:35:48 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>

char	*get_next_line(int fd);
int		ft_strlen(char *s);
char	*ft_strjoin(char *line, char *buffer);
char	*get_line_from_buffer(char *line, int *fd);
int		find_newline(char *line);
char	*get_first_line(char *line);
char	*update_line(char *line, char **first_line);

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 10000
# endif

#endif