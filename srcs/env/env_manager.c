#include "../incs/42sh.h"
#include "../incs/env.h"

void	print_var(t_var *var)
{
	ft_putstr_fd(var->name, 1);
	ft_putstr_fd("=", 1);
	ft_putstr_fd(var->value, 1);
	ft_putstr_fd("\n", 1);
}

void	print_var_table(t_var_table *table) // DEBUG
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
				print_var(tmp);
				tmp = tmp->next;
			}
		}
	}
	ft_putstr_fd("*****************************\n", 0);
}

char	*get_variable(t_var_table *table, const char *name)
{
	unsigned int	hash;
	t_var			*tmp;

	hash = hash_string(name, VAR_HASH_SIZE);
	tmp = table->buckets[hash];

	while(tmp)
	{
		if (!ft_strcmp(tmp->name, name))
		{
			return (tmp->value);
		}
		
		tmp = tmp->next;
	}

	return (NULL);
}

static void	handle_shell_level(t_var_table *table)
{
	char	*lvl = get_variable(table, "SHLVL");
	if (lvl)
	{
		char *new_lvl = ft_itoa(ft_atoi(lvl) + 1);
		set_variable(table, "SHLVL", new_lvl, 1, 1);
		free(new_lvl);
	}
	else
	{
		set_variable(table, "SHLVL", "1", 1, 1);
	}
}

static void	handle_pwd(t_var_table *table)
{
	char	*pwd = get_variable(table, "PWD");
	if (!pwd)
	{
		char fresh_pwd[1024] ;
		getcwd(fresh_pwd, sizeof(fresh_pwd));
		set_variable(table, "PWD", fresh_pwd, 1, 1);
	}
}

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
				set_variable(table, name, value, 1, 1);

			free(name);
			free(value);
		}
		i++;
	}

	handle_shell_level(table);
	handle_pwd(table);

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

int	set_variable(t_var_table *table, const char *name, const char *value, int exported, int was_equalized)
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
			return (0);
		}
		current = current->next;
	}

	t_var	*new_var = malloc(sizeof(t_var));
	if (!new_var)
	{
		//TODO handle malloc fails -> Garbage collector
		return (1);
	}

	new_var->name = ft_strdup((char *)name);

	if(was_equalized)
	{
		new_var->value = value == NULL ? ft_strdup("") : ft_strdup((char *)value);
	} else
	{
		new_var->value = NULL;
	}
	
	new_var->exported = exported;
	new_var->next = table->buckets[hash];
	table->buckets[hash] = new_var;

	return (0);
}

int	count_stored_env_variables(t_var_table *table)
{
	int		count = 0;
	t_var	*current;

	if (!table)
		return (0);

	for (int i = 0; i < VAR_HASH_SIZE; i++)
	{
		current = table->buckets[i];
		while (current)
		{
			count++;
			current = current->next;
		}
	}

	return (count);
}

int compare_vars(const void *a, const void *b)
{
	t_var *var_a = *(t_var**)a;
	t_var *var_b = *(t_var**)b;
	return ft_strcmp(var_a->name, var_b->name);
}

int mark_variable_as_exported(t_var_table *table, const char *name)
{
	unsigned int hash = hash_string(name, VAR_HASH_SIZE);
	t_var *current = table->buckets[hash];
	
	while (current)
	{
		if (ft_strcmp(current->name, name) == 0)
		{
			current->exported = 1;
			return (0);
		}
		current = current->next;
	}
	
	return (1);
}

void	free_var_table(t_var_table *table)
{
	unsigned int	i;
	t_var			*current;
	t_var			*next;

	if (!table)
		return;

	i = 0;
	while (i < VAR_HASH_SIZE)
	{
		current = table->buckets[i];
		while (current)
		{
			next = current->next;
			free(current->name);
			if (current->value) free(current->value);
			free(current);
			current = next;
		}
		table->buckets[i] = NULL;
		i++;
	}
	free(table);
}

t_var	**get_sorted_variable_refs(t_var_table *table, size_t *count)
{
	*count = 0;
	for (size_t i = 0; i < VAR_HASH_SIZE; i++) {
		t_var *current = table->buckets[i];
		while (current) {
			if (current->exported) {
				(*count)++;
			}
			current = current->next;
		}
	}
	
	t_var **refs = malloc(*count * sizeof(t_var*));
	if (!refs) return NULL;
	
	size_t var_idx = 0;
	for (size_t i = 0; i < VAR_HASH_SIZE; i++) {
		t_var *current = table->buckets[i];
		while (current) {
			if (current->exported) {
				refs[var_idx++] = current;
			}
			current = current->next;
		}
	}
	
	qsort(refs, *count, sizeof(t_var*), compare_vars);
	return refs;
}
