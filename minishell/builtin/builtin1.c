/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:56:44 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 03:56:50 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void remove_env(t_env **env_list, char *key)
{
 t_env *prev;
 t_env *current;

 prev = NULL;
 current = *env_list;
 while (current)
 {
  if (ft_strcmp(current->key, key) == 0)
  {
   if (prev)
    prev->next = current->next;
   else
    *env_list = current->next;
   free(current->key);
   free(current->value);
   free(current);
   break ;
  }
  prev = current;
  current = current->next;
 }
}

int exec_unset(t_token *argv, t_env *env_list)
{
 t_token *tmp;

 tmp = argv->next;
 while (tmp)
 {
  remove_env(&env_list, tmp->value);
  tmp = tmp->next;
 }
 return (0);
}

static void print_env_list(t_env *env_list)
{
 while (env_list)
 {
  if (env_list->value)
   printf("%s=%s\n", env_list->key, env_list->value);
  env_list = env_list->next;
 }
}

int exec_env(t_token *argv, t_env *env_list)
{
 char *path;
 char **args;
 char **environ;

 if (!argv->next)
 {
  print_env_list(env_list);
  return (0);
 }
 path = get_path(argv->next->value, env_list);
 if (!path)
 {
  write(2, "env: '", 6);
  write(2, argv->next->value, ft_strlen(argv->next->value));
  write(2, "': No such file or directory\n", 29);
  return (127);
 }
 args = to_list(argv->next);
 environ = env_to_environ(env_list);
 if (!args || !environ)
  return (free(path), free_str_list(args), free_str_list(environ), 1);
 execve(path, args, environ);
 free_all_arg(path, args, environ);
 return (127);
}

long ft_atol(const char *str)
{
 int  i;
 int  sign;
 long result;

 i = 0;
 sign = 1;
 result = 0;
 while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
  i++;
 if (str[i] == '-' || str[i] == '+')
 {
  if (str[i] == '-')
   sign = -1;
  i++;
 }
 while (str[i] >= '0' && str[i] <= '9')
 {
  result = result * 10 + (str[i] - '0');
  i++;
 }
 return (result * sign);
}
