#ifndef DATA_STRUCTURES_H
# define DATA_STRUCTURES_H

# define VAR_HASH_SIZE 256
# define CMD_HASH_SIZE 64

# include <unistd.h>
# include <signal.h>

typedef enum e_token_type {
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_SEMICOLON,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_REDIR_HEREDOC,
	TOKEN_REDIR_FD_IN,
	TOKEN_REDIR_FD_OUT,
	TOKEN_BACKGROUND,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_EOF
} t_token_type;

typedef struct s_token {
	t_token_type type;
	char *value;
	struct s_token *next;
} t_token;

typedef enum e_ast_type {
	AST_COMMAND,
	AST_PIPE,
	AST_SEMICOLON,
	AST_AND,
	AST_OR,
	AST_REDIRECT,
	AST_BACKGROUND,
	AST_SUBSHELL
} t_ast_type;

typedef struct s_redir {
	int type;
	char *file;
	int fd;
} t_redir;

typedef struct s_ast_node {
	t_ast_type type;
	
	char **argv;
	t_redir *redirs;
	int redir_count;
	
	struct s_ast_node *left;
	struct s_ast_node *right;
	
	int background;
} t_ast_node;

typedef enum e_job_status {
	JOB_RUNNING,
	JOB_STOPPED,
	JOB_DONE
} t_job_status;

typedef struct s_job {
	int job_id;
	pid_t pgid;
	char *command;
	t_job_status status;
	int background;
	struct s_job *next;
} t_job;

typedef struct s_var {
	char *name;
	char *value;
	int exported;
	struct s_var *next;
} t_var;

typedef struct s_var_table {
	t_var *buckets[VAR_HASH_SIZE];
	char **envp;
} t_var_table;

typedef struct s_cmd_entry {
	char *command;
	char *path;
	int hits;
	struct s_cmd_entry *next;
} t_cmd_entry;

typedef struct s_cmd_table {
	t_cmd_entry *buckets[CMD_HASH_SIZE];
	int total_entries;
} t_cmd_table;

typedef struct s_echo_config {
	int	suppress_newline;
	int	enable_escapes;
	int	disable_escapes;
	int	stop_processing;
} t_echo_config;

typedef struct s_shell {
	t_var_table			*vars;
	
	t_job				*jobs;
	int 				next_job_id;
	pid_t				shell_pgid;
	int 				shell_terminal;
	int 				interactive;
	
	char 				*history_file;
	
	t_cmd_table 		*cmd_cache;
	
	int 				last_exit_status;
	
	struct sigaction 	old_sigint;
	struct sigaction 	old_sigtstp;

	char				*prompt;
	char				*ifs;
	
} t_shell;

t_var_table		*init_var_table(void);
void			free_var_table(t_var_table *table);
int				set_variable(t_var_table *table, const char *name, const char *value, int exported);
char			*get_variable(t_var_table *table, const char *name);
int				unset_variable(t_var_table *table, const char *name);
t_var_table		*capture_initial_environment(char **env);

t_cmd_table		*init_cmd_table(void);
void			free_cmd_table(t_cmd_table *table);
char			*get_command_path(t_cmd_table *table, const char *command);
int				cache_command_path(t_cmd_table *table, const char *command, const char *path);
void			clear_cmd_cache(t_cmd_table *table);

unsigned int	hash_string(const char *str, unsigned int table_size);

#endif
