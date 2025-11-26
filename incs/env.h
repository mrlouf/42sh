#ifndef ENV_H
# define ENV_H
# include "data_structures.h"

t_var_table	*capture_initial_environment(char **env);
unsigned int	hash_string(const char *str, unsigned int table_size);
t_var_table	*init_var_table(void);
int				set_variable(t_var_table *table, const char *name, const char *value, int exported);
char			*get_variable(t_var_table *table, const char *name);
void			free_var_table(t_var_table *table);

#endif
