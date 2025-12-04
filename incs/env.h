#ifndef ENV_H
# define ENV_H
# include "data_structures.h"

t_var_table		*capture_initial_environment(char **env);
unsigned int	hash_string(const char *str, unsigned int table_size);
t_var_table		*init_var_table(void);
int				set_variable(t_var_table *table, const char *name, const char *value, int exported, int was_equalized);
int				unset_variable(t_var_table *table, const char *name);
int				variable_exists(t_var_table *table, const char *name);
char			*get_variable(t_var_table *table, const char *name);
int				count_stored_env_variables(t_var_table *table);
int				compare_vars(const void *a, const void *b);
int				mark_variable_as_exported(t_var_table *table, const char *name);
int				mark_variable_as_readonly(t_var_table *table, const char *name);
int				is_variable_exported(t_var_table *table, const char *name);
int				is_variable_readonly(t_var_table *table, const char *name);
t_var			**get_all_sorted_refs(t_var_table *table, size_t *count);
t_var			**get_sorted_exported_refs(t_var_table *table, size_t *count);
t_var			**get_sorted_readonly_refs(t_var_table *table, size_t *count);
void			free_var_table(t_var_table *table);

// PRINTING
void			print_var(t_var *var);
void			print_var_table(t_var_table	*table);

#endif
