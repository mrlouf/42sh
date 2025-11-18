#include "../incs/42sh.h"

t_var_table	*capture_initial_environment(char **env)
{
	t_var_table	*table;
	int			i;
	char		*eq_pos;
	char		*name;
	char		*value;
	int			name_len;

	table = init_var_table();
	if (!table)
		return (NULL);

	i = 0;
	while (env[i])
	{
		eq_pos = ft_strchr(env[i], '=');
		if (eq_pos)
		{
			name_len = eq_pos - env[i];
			name = ft_strndup(env[i], name_len);
			value = ft_strdup(eq_pos + 1);


			if (name && value)
				set_variable(table, name, value, 1);

			free(name);
			free(value);
		}
		i++;
	}

	return (table);
}

unsigned int	hash_string(const char *str, unsigned int table_size)
{
	unsigned int	hash = 5381;
	int				c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return (hash % table_size);
}

t_var_table	*init_var_table(void)
{
	t_var_table	*table;
	int			i;

	table = malloc(sizeof(t_var_table));
	if (!table)
		return (NULL);

	i = 0;
	while (i < VAR_HASH_SIZE)
	{
		table->buckets[i] = NULL;
		i++;
	}
	table->envp = NULL;

	return (table);
}

int	set_variable(t_var_table *table, const char *name, const char *value, int exported)
{
	unsigned int	hash = hash_string(name, VAR_HASH_SIZE);
	t_var			*current = table->buckets[hash];

	while (current)
	{
		if (ft_strcmp(current->name, name) == 0)
		{
			free(current->value);
			current->value = ft_strdup((char *)value);
			current->exported = exported;
			return (1);
		}
		current = current->next;
	}

	t_var	*new_var = malloc(sizeof(t_var));
	if (!new_var)
	{
		//TODO handle malloc fails -> Garbage collector
		return (0);
	}

	new_var->name = ft_strdup((char *)name);
	new_var->value = ft_strdup((char *)value);
	new_var->exported = exported;
	new_var->next = table->buckets[hash]; // Beginning of chain
	table->buckets[hash] = new_var;

	return (1);
}

// DEBUG
void	print_captured_env(t_var_table *table)
{
	ft_putstr_fd("*****************************\n", 0);
	ft_putstr_fd("Captured initial env:\n", 0);
	
	for (int i = 0; i < VAR_HASH_SIZE; i++)
	{
		if (table->buckets[i] != NULL)
		{
			t_var *tmp = table->buckets[i];

			while (tmp)
			{
				print_env_variable(tmp);
				tmp = tmp->next;
			}
		}
	}
	ft_putstr_fd("*****************************\n", 0);
}

void	print_env_variable(t_var *var)
{
	ft_putstr_fd(var->name, 0);
	ft_putstr_fd("=", 0);
	ft_putstr_fd(var->value, 0);
	ft_putstr_fd("\n", 0);
}
