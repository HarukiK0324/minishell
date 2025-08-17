/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin7.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 00:57:00 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/17 10:57:53 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int only_contains(char *str, char *chars)
{
 int i;
 int j;

 i = 1;
 while (str[i] != '\0')
 {
  j = 0;
  while (chars[j] != '\0')
  {
   if (str[i] == chars[j])
    break ;
   j++;
  }
  if (chars[j] == '\0')
   return (0);
  i++;
 }
 return (1);
}

int exec_echo(t_token *argv)
{
 int  newline;
 t_token *tmp;

 newline = 1;
 tmp = argv->next;
 while (tmp && tmp->value[0] == '-' && tmp->value[1] != '\0'
  && only_contains(tmp->value, "n"))
 {
  newline = 0;
  tmp = tmp->next;
 }
 while (tmp)
 {
  printf("%s", tmp->value);
  tmp = tmp->next;
  if (tmp)
   printf(" ");
 }
 if (newline)
  printf("\n");
 return (0);
}

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

void handle_append_export(t_env *env_list, char *key, char *value)
{
 t_env *existing;
 char *new_value;

 existing = find_env(env_list, key);
 if (existing && existing->value)
 {
  new_value = concat_strings(existing->value, value);
  if (!new_value)
   return ;
  update_env(env_list, key, new_value);
  free(new_value);
 }
 else
  update_env(env_list, key, value);
}
