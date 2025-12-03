#include <criterion/criterion.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../incs/42sh.h" // Adjust path as needed
#include "../incs/token.h" // Adjust path as needed
#include "../incs/data_structures.h" // Adjust path as needed

TestSuite(tokens_basic);

// Helper function to create test shell
static t_shell *create_test_shell(void) {
	t_shell *shell = calloc(1, sizeof(t_shell));
	if (!shell)
		return NULL;
	
	shell->vars = calloc(1, sizeof(t_var_table));
	if (!shell->vars) {
		free(shell);
		return NULL;
	}
	
	// Initialize with basic environment variables
	char *current_pwd = getcwd(NULL, 0);
	if (current_pwd) {
		set_variable(shell->vars, "PWD", current_pwd, 1);
		free(current_pwd);
	}
	set_variable(shell->vars, "HOME", getenv("HOME"), 1);
	
	return shell;
}

// Helper function to cleanup test shell
static void cleanup_test_shell(t_shell *shell) {
	if (!shell)
		return;
    
    if (shell->vars)
        free(shell->vars);
    free(shell);
}

	// Helper function to count tokens
	static int count_tokens(token_t *tokens) {
		if (!tokens)
			return 0;
		
		int count = 0;
		while (tokens[count].type != TOKEN_EOF && tokens[count].type != TOKEN_NULL) {
			count++;
		}
		return count;
	}

	// Helper function to print tokens for debugging
	static void debug_print_tokens(token_t *tokens) {
		if (!tokens) {
			printf("Tokens: NULL\n");
			return;
		}
		
		printf("Tokens: ");
		for (int i = 0; tokens[i].type != TOKEN_EOF && tokens[i].type != TOKEN_NULL; i++) {
			printf("[%d:'%s'] ", tokens[i].type, tokens[i].value ? tokens[i].value : "NULL");
		}
		printf("\n");
	}

	// Basic single token tests
Test(tokens_basic, single_word_token) {
	t_shell *shell = create_test_shell();
    cr_assert_not_null(shell, "Failed to create test shell");

	char *input = "hello";
	token_t *tokens = tokenize(input);
		
	cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 1, "Should have exactly 1 token");
	cr_assert_str_eq(tokens[0].value, "hello", "First token should be 'hello'");
	cr_assert_eq(tokens[0].type, TOKEN_WORD, "Token type should be WORD");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, empty_string) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "";
		token_t *tokens = tokenize(input);
		
		cr_assert_eq(count_tokens(tokens), 0, "Empty string should produce 0 tokens");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, whitespace_only) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "   \t  \n  ";
		token_t *tokens = tokenize(input);
		
		cr_assert_eq(count_tokens(tokens), 0, "Whitespace only should produce 0 tokens");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	// Multiple word tests
	Test(tokens_basic, two_words) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "hello world";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 2, "Should have exactly 2 tokens");
		cr_assert_str_eq(tokens[0].value, "hello", "First token should be 'hello'");
		cr_assert_str_eq(tokens[1].value, "world", "Second token should be 'world'");
		cr_assert_eq(tokens[0].type, TOKEN_WORD, "First token type should be WORD");
		cr_assert_eq(tokens[1].type, TOKEN_WORD, "Second token type should be WORD");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, multiple_spaces_between_words) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "hello    world   test";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 3, "Should have exactly 3 tokens");
		cr_assert_str_eq(tokens[0].value, "hello", "First token should be 'hello'");
		cr_assert_str_eq(tokens[1].value, "world", "Second token should be 'world'");
		cr_assert_str_eq(tokens[2].value, "test", "Third token should be 'test'");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	// Operator tests
	Test(tokens_basic, pipe_operator) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "ls | grep test";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 4, "Should have exactly 4 tokens");
		cr_assert_str_eq(tokens[0].value, "ls", "First token should be 'ls'");
		cr_assert_eq(tokens[0].type, TOKEN_WORD, "First token should be WORD");
		cr_assert_str_eq(tokens[1].value, "|", "Second token should be '|'");
		cr_assert_eq(tokens[1].type, TOKEN_PIPE, "Second token should be PIPE");
		cr_assert_str_eq(tokens[2].value, "grep", "Third token should be 'grep'");
		cr_assert_str_eq(tokens[3].value, "test", "Fourth token should be 'test'");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, redirection_operators) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "echo hello > output.txt";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 4, "Should have exactly 4 tokens");
		cr_assert_str_eq(tokens[0].value, "echo", "First token should be 'echo'");
		cr_assert_str_eq(tokens[1].value, "hello", "Second token should be 'hello'");
		cr_assert_str_eq(tokens[2].value, ">", "Third token should be '>'");
		cr_assert_eq(tokens[2].type, TOKEN_REDIRECT_OUT, "Third token should be REDIRECT_OUT");
		cr_assert_str_eq(tokens[3].value, "output.txt", "Fourth token should be 'output.txt'");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, input_redirection) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "cat < input.txt";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 3, "Should have exactly 3 tokens");
		cr_assert_str_eq(tokens[0].value, "cat", "First token should be 'cat'");
		cr_assert_str_eq(tokens[1].value, "<", "Second token should be '<'");
		cr_assert_eq(tokens[1].type, TOKEN_REDIRECT_IN, "Second token should be REDIRECT_IN");
		cr_assert_str_eq(tokens[2].value, "input.txt", "Third token should be 'input.txt'");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, append_redirection) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "echo hello >> output.txt";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 4, "Should have exactly 4 tokens");
		cr_assert_str_eq(tokens[2].value, ">>", "Third token should be '>>'");
		cr_assert_eq(tokens[2].type, TOKEN_REDIRECT_APPEND, "Third token should be REDIRECT_APPEND");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	// Quote tests
	Test(tokens_basic, single_quotes) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "echo 'hello world'";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 2, "Should have exactly 2 tokens");
		cr_assert_str_eq(tokens[0].value, "echo", "First token should be 'echo'");
		cr_assert_str_eq(tokens[1].value, "hello world", "Second token should be 'hello world'");
		cr_assert_eq(tokens[1].type, TOKEN_WORD, "Quoted string should be WORD type");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, double_quotes) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "echo \"hello world\"";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 2, "Should have exactly 2 tokens");
		cr_assert_str_eq(tokens[0].value, "echo", "First token should be 'echo'");
		cr_assert_str_eq(tokens[1].value, "hello world", "Second token should be 'hello world'");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	// Complex command tests
	Test(tokens_basic, complex_pipeline) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "ls -la | grep test | wc -l";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 8, "Should have exactly 8 tokens");
		
		// Verify structure: ls -la | grep test | wc -l
		cr_assert_str_eq(tokens[0].value, "ls", "Token 0 should be 'ls'");
		cr_assert_str_eq(tokens[1].value, "-la", "Token 1 should be '-la'");
		cr_assert_eq(tokens[2].type, TOKEN_PIPE, "Token 2 should be PIPE");
		cr_assert_str_eq(tokens[3].value, "grep", "Token 3 should be 'grep'");
		cr_assert_str_eq(tokens[4].value, "test", "Token 4 should be 'test'");
		cr_assert_eq(tokens[5].type, TOKEN_PIPE, "Token 5 should be PIPE");
		cr_assert_str_eq(tokens[6].value, "wc", "Token 6 should be 'wc'");
		cr_assert_str_eq(tokens[7].value, "-l", "Token 7 should be '-l'");
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, command_with_multiple_redirections) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "sort < input.txt > output.txt 2> error.log";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 7, "Should have exactly 7 tokens");
		
		// Verify the redirection operators are correctly identified
		cr_assert_eq(tokens[1].type, TOKEN_REDIRECT_IN, "Token 1 should be REDIRECT_IN");
		cr_assert_eq(tokens[3].type, TOKEN_REDIRECT_OUT, "Token 3 should be REDIRECT_OUT");
		// Note: 2> might be tokenized as separate tokens or as one depending on implementation
		
		free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	// Edge cases
	Test(tokens_basic, operators_without_spaces) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "echo hello>output.txt";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		// The exact count depends on your tokenizer implementation
		// It should separate operators even without spaces
		int token_count = count_tokens(tokens);
		cr_assert_geq(token_count, 3, "Should have at least 3 tokens");
		
	free_tokens(tokens);
		cleanup_test_shell(shell);
	}

	Test(tokens_basic, semicolon_separator) {
		t_shell *shell = create_test_shell();
		cr_assert_not_null(shell, "Failed to create test shell");

		char *input = "echo hello; ls -la";
		token_t *tokens = tokenize(input);
		
		cr_assert_not_null(tokens, "Tokens should not be null");
		cr_assert_eq(count_tokens(tokens), 5, "Should have exactly 5 tokens");
		
		// Find the semicolon token
		bool found_semicolon = false;
		for (int i = 0; i < count_tokens(tokens); i++) {
			if (tokens[i].type == TOKEN_SEMICOLON) {
				found_semicolon = true;
				cr_assert_str_eq(tokens[i].value, ";", "Semicolon token value should be ';'");
				break;
			}
		}
		cr_assert(found_semicolon, "Should find semicolon token");
		
		free_tokens(tokens);
	cleanup_test_shell(shell);
	}
}