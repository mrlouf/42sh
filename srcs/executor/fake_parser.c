#include "../../incs/42sh.h"
#include "../../incs/executor.h"
#include <string.h>

// DEBUG / DEV file for executor development
// This file provides a fake parser to test executor features
// TODO: Remove this file when real parser is integrated

static void free_redirections(t_redir *redirs, int count)
{
	if (!redirs)
		return;
		
	for (int i = 0; i < count; i++)
	{
		if (redirs[i].file)
			free(redirs[i].file);
	}
	free(redirs);
}

static char **parse_command_tokens(char *input, int *token_count)
{
	char **tokens = ft_split(input, ' ');
	if (!tokens)
	{
		*token_count = 0;
		return (NULL);
	}
	
	// Count tokens
	*token_count = 0;
	while (tokens[*token_count])
		(*token_count)++;
	
	return (tokens);
}

t_ast_node *fake_parse_input(char *input)
{
	if (!input || !*input)
		return (NULL);
	
	// Trim whitespace
	char *trimmed = ft_strtrim(input, " \t\n");
	if (!trimmed || !*trimmed)
	{
		if (trimmed)
			free(trimmed);
		return (NULL);
	}
	
	t_ast_node *node = malloc(sizeof(t_ast_node));
	if (!node)
	{
		free(trimmed);
		return (NULL);
	}
	
	// Initialize node
	node->type = AST_COMMAND;
	node->argv = NULL;
	node->redirs = NULL;
	node->redir_count = 0;
	node->left = NULL;
	node->right = NULL;
	node->background = 0;
	
	// Parse tokens
	int token_count;
	char **tokens = parse_command_tokens(trimmed, &token_count);
	free(trimmed);
	
	if (!tokens || token_count == 0)
	{
		free(node);
		return (NULL);
	}
	
	// Count redirections and regular tokens separately
	int cmd_count = 0;
	int redir_count = 0;
	
	for (int i = 0; i < token_count; i++)
	{
		if (ft_strcmp(tokens[i], ">") == 0 || 
			ft_strcmp(tokens[i], ">>") == 0 || 
			ft_strcmp(tokens[i], "<") == 0 ||
			ft_strcmp(tokens[i], "2>") == 0 ||
			ft_strcmp(tokens[i], "<<") == 0 ||
			ft_strcmp(tokens[i], "<<<") == 0 ||
			ft_strcmp(tokens[i], "2>>") == 0 ||
			ft_strcmp(tokens[i], "&>") == 0)
		{
			if (i + 1 >= token_count)
			{
				ft_putstr_fd("42sh: syntax error near unexpected token `newline'\n", 2);
				ft_array_free((void**)tokens);
				free(node);
				return (NULL);
			}
			redir_count++;
			i++; // Skip filename
		}
		else if (ft_strcmp(tokens[i], ">&2") == 0 || 
				 ft_strcmp(tokens[i], "2>&1") == 0)
		{
			redir_count++;
			// No i++ since these don't require filenames
		}
		else
		{
			cmd_count++;
		}
	}
	
	if (cmd_count == 0)
	{
		ft_putstr_fd("42sh: syntax error: no command specified\n", 2);
		ft_array_free((void**)tokens);
		free(node);
		return (NULL);
	}
	
	// Allocate arrays
	node->argv = malloc(sizeof(char*) * (cmd_count + 1));
	if (redir_count > 0)
		node->redirs = malloc(sizeof(t_redir) * redir_count);
	else
		node->redirs = NULL;
	
	if (!node->argv || (redir_count > 0 && !node->redirs))
	{
		free(node->argv);
		free(node->redirs);
		ft_array_free((void**)tokens);
		free(node);
		return (NULL);
	}
	
	// Fill arrays
	int cmd_idx = 0;
	int redir_idx = 0;
	
	for (int i = 0; i < token_count; i++)
	{
		if (ft_strcmp(tokens[i], ">") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_OUT;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip filename
		}
		else if (ft_strcmp(tokens[i], ">>") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_APPEND;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip filename
		}
		else if (ft_strcmp(tokens[i], "<") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_IN;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip filename
		}
		else if (ft_strcmp(tokens[i], "2>") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_STDERR;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip filename
		}
		else if (ft_strcmp(tokens[i], "<<") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_HEREDOC;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip delimiter
		}
		else if (ft_strcmp(tokens[i], "<<<") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_HERESTRING;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip string
		}
		else if (ft_strcmp(tokens[i], "2>>") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_STDERR_APPEND;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip filename
		}
		else if (ft_strcmp(tokens[i], "&>") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_BOTH;
			node->redirs[redir_idx].file = ft_strdup(tokens[i + 1]);
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			i++; // Skip filename
		}
		else if (ft_strcmp(tokens[i], ">&2") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_STDOUT_TO_STDERR;
			node->redirs[redir_idx].file = ft_strdup("2"); // Special case
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			// No i++ since >&2 is a single token
		}
		else if (ft_strcmp(tokens[i], "2>&1") == 0)
		{
			node->redirs[redir_idx].type = REDIRECT_STDERR_TO_STDOUT;
			node->redirs[redir_idx].file = ft_strdup("1"); // Special case
			node->redirs[redir_idx].fd = -1;
			redir_idx++;
			// No i++ since 2>&1 is a single token
		}
		else
		{
			node->argv[cmd_idx] = ft_strdup(tokens[i]);
			cmd_idx++;
		}
	}
	
	node->argv[cmd_idx] = NULL;
	node->redir_count = redir_count;
	
	ft_array_free((void**)tokens);
	return (node);
}

void fake_free_ast_node(t_ast_node *node)
{
	if (!node)
		return;
		
	// Free argv
	if (node->argv)
	{
		for (int i = 0; node->argv[i]; i++)
		{
			free(node->argv[i]);
		}
		free(node->argv);
	}
	
	// Free redirections
	free_redirections(node->redirs, node->redir_count);
	
	// Free child nodes (for when we add pipes, etc.)
	if (node->left)
		fake_free_ast_node(node->left);
	if (node->right)
		fake_free_ast_node(node->right);
	
	free(node);
}

// DEBUG function: Print AST node for debugging
void fake_print_ast_node(t_ast_node *node)
{
	if (!node)
		return;
		
	ft_putstr_fd("AST Node:\n", 1);
	ft_putstr_fd("  Command: ", 1);
	if (node->argv && node->argv[0])
	{
		for (int i = 0; node->argv[i]; i++)
		{
			ft_putstr_fd(node->argv[i], 1);
			if (node->argv[i + 1])
				ft_putstr_fd(" ", 1);
		}
	}
	ft_putstr_fd("\n", 1);
	
	ft_putstr_fd("  Redirections: ", 1);
	for (int i = 0; i < node->redir_count; i++)
	{
		if (node->redirs[i].type == REDIRECT_OUT)
			ft_putstr_fd(" > ", 1);
		else if (node->redirs[i].type == REDIRECT_APPEND)
			ft_putstr_fd(" >> ", 1);
		else if (node->redirs[i].type == REDIRECT_IN)
			ft_putstr_fd(" < ", 1);
		else if (node->redirs[i].type == REDIRECT_STDERR)
			ft_putstr_fd(" 2> ", 1);
		else if (node->redirs[i].type == REDIRECT_HEREDOC)
			ft_putstr_fd(" << ", 1);
		else if (node->redirs[i].type == REDIRECT_HERESTRING)
			ft_putstr_fd(" <<< ", 1);
		else if (node->redirs[i].type == REDIRECT_STDERR_APPEND)
			ft_putstr_fd(" 2>> ", 1);
		else if (node->redirs[i].type == REDIRECT_BOTH)
			ft_putstr_fd(" &> ", 1);
		else if (node->redirs[i].type == REDIRECT_STDOUT_TO_STDERR)
			ft_putstr_fd(" >&2", 1);
		else if (node->redirs[i].type == REDIRECT_STDERR_TO_STDOUT)
			ft_putstr_fd(" 2>&1", 1);
		ft_putstr_fd(node->redirs[i].file, 1);
	}
	ft_putstr_fd("\n", 1);
}
