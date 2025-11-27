#include "../../incs/42sh.h"
#include "../../incs/data_structures.h"
#include "../../incs/env.h"

t_cmd_table	*init_cmd_table(void)
{
	t_cmd_table *table;
	int i;

	table = malloc(sizeof(t_cmd_table));
	if (!table)
		return (NULL);
	
	i = 0;
	while (i < CMD_HASH_SIZE)
	{
		table->buckets[i] = NULL;
		i++;
	}
	table->total_entries = 0;
	return (table);
}

static void	free_cmd_entry(t_cmd_entry *entry)
{
	if (entry)
	{
		free(entry->command);
		free(entry->path);
		free(entry);
	}
}

void	free_cmd_table(t_cmd_table *table)
{
	t_cmd_entry *entry;
	t_cmd_entry *next;
	int i;

	if (!table)
		return;
	
	i = 0;
	while (i < CMD_HASH_SIZE)
	{
		entry = table->buckets[i];
		while (entry)
		{
			next = entry->next;
			free_cmd_entry(entry);
			entry = next;
		}
		i++;
	}
	free(table);
}

char	*get_command_path(t_cmd_table *table, const char *command)
{
	t_cmd_entry *entry;
	unsigned int hash;

	if (!table || !command)
		return (NULL);
	
	hash = hash_string(command, CMD_HASH_SIZE);
	entry = table->buckets[hash];
	
	while (entry)
	{
		if (ft_strcmp(entry->command, command) == 0)
		{
			entry->hits++;
			return (ft_strdup(entry->path));
		}
		entry = entry->next;
	}
	return (NULL);
}

int	cache_command_path(t_cmd_table *table, const char *command, const char *path)
{
	t_cmd_entry *entry;
	unsigned int hash;

	if (!table || !command || !path)
		return (0);
	
	hash = hash_string(command, CMD_HASH_SIZE);
	entry = table->buckets[hash];
	while (entry)
	{
		if (ft_strcmp(entry->command, command) == 0)
		{
			free(entry->path);
			entry->path = ft_strdup((char *)path);
			return (entry->path != NULL);
		}
		entry = entry->next;
	}
	
	entry = malloc(sizeof(t_cmd_entry));
	if (!entry)
		return (0);
	
	entry->command = ft_strdup((char *)command);
	entry->path = ft_strdup((char *)path);
	entry->hits = 1;
	
	if (!entry->command || !entry->path)
	{
		free_cmd_entry(entry);
		return (0);
	}
	
	entry->next = table->buckets[hash];
	table->buckets[hash] = entry;
	table->total_entries++;
	
	return (1);
}

void	clear_cmd_cache(t_cmd_table *table)
{
	t_cmd_entry *entry;
	t_cmd_entry *next;
	int i;

	if (!table)
		return;
	
	i = 0;
	while (i < CMD_HASH_SIZE)
	{
		entry = table->buckets[i];
		while (entry)
		{
			next = entry->next;
			free_cmd_entry(entry);
			entry = next;
		}
		table->buckets[i] = NULL;
		i++;
	}
	table->total_entries = 0;
}
