/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin8.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hkasamat <hkasamat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 15:42:32 by hkasamat          #+#    #+#             */
/*   Updated: 2025/08/09 15:42:42 by hkasamat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void handle_append_export(t_env *env_list, char *key, char *value)
{
 t_env *existing;
 char *new_value;

 existing = find_env(env_list, key);
 if (existing && existing->value)
 {
  new_value = append(existing->value, value, '\0');
  update_env(env_list, key, new_value);
  free(new_value);
 }
 else
  update_env(env_list, key, value);
}
