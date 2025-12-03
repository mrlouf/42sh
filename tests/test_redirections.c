#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../incs/42sh.h"
#include "../incs/executor.h"

/* static int check_file_content(const char *filename, const char *expected)
{
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return 0;
    
    char buffer[1024] = {0};
    ssize_t bytes = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    
    if (bytes < 0)
        return 0;
    
    return strcmp(buffer, expected) == 0;
} */

static void cleanup_test_files(void)
{
    unlink("test_input.txt");
    unlink("test_output.txt");
    unlink("test_append.txt");
    unlink("test_stderr.txt");
    unlink("test_multiple1.txt");
    unlink("test_multiple2.txt");
}

Test(redirections, basic_input_redirection)
{
    cleanup_test_files();
    
    int fd = open("test_input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, "hello from file\n", 16);
    close(fd);
    

    t_ast_node *ast = fake_parse_input("cat < test_input.txt");
    cr_assert_not_null(ast, "AST should not be null");
    cr_assert_eq(ast->redir_count, 1, "Should have one redirection");
    cr_assert_eq(ast->redirs[0].type, REDIRECT_IN, "Should be input redirection");
    cr_assert_str_eq(ast->redirs[0].file, "test_input.txt", "File should match");
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}

Test(redirections, basic_output_redirection)
{
    cleanup_test_files();
    
    t_ast_node *ast = fake_parse_input("echo hello world > test_output.txt");
    cr_assert_not_null(ast, "AST should not be null");
    cr_assert_eq(ast->redir_count, 1, "Should have one redirection");
    cr_assert_eq(ast->redirs[0].type, REDIRECT_OUT, "Should be output redirection");
    cr_assert_str_eq(ast->redirs[0].file, "test_output.txt", "File should match");
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}

Test(redirections, append_redirection)
{
    cleanup_test_files();
    
    t_ast_node *ast = fake_parse_input("echo append test >> test_append.txt");
    cr_assert_not_null(ast, "AST should not be null");
    cr_assert_eq(ast->redir_count, 1, "Should have one redirection");
    cr_assert_eq(ast->redirs[0].type, REDIRECT_APPEND, "Should be append redirection");
    cr_assert_str_eq(ast->redirs[0].file, "test_append.txt", "File should match");
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}

Test(redirections, stderr_redirection)
{
    cleanup_test_files();

    t_ast_node *ast = fake_parse_input("ls nonexistent 2> test_stderr.txt");
    cr_assert_not_null(ast, "AST should not be null");
    cr_assert_eq(ast->redir_count, 1, "Should have one redirection");
    cr_assert_eq(ast->redirs[0].type, REDIRECT_STDERR, "Should be stderr redirection");
    cr_assert_str_eq(ast->redirs[0].file, "test_stderr.txt", "File should match");
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}

Test(redirections, multiple_redirections)
{
    cleanup_test_files();

    int fd = open("test_input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, "input content\n", 14);
    close(fd);

    t_ast_node *ast = fake_parse_input("cat < test_input.txt > test_multiple1.txt");
    cr_assert_not_null(ast, "AST should not be null");
    cr_assert_eq(ast->redir_count, 2, "Should have two redirections");

    int input_found = 0, output_found = 0;
    for (int i = 0; i < ast->redir_count; i++)
    {
        if (ast->redirs[i].type == REDIRECT_IN)
        {
            input_found = 1;
            cr_assert_str_eq(ast->redirs[i].file, "test_input.txt", "Input file should match");
        }
        else if (ast->redirs[i].type == REDIRECT_OUT)
        {
            output_found = 1;
            cr_assert_str_eq(ast->redirs[i].file, "test_multiple1.txt", "Output file should match");
        }
    }
    cr_assert(input_found, "Input redirection should be found");
    cr_assert(output_found, "Output redirection should be found");
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}

Test(redirections, multiple_same_type_redirections)
{
    cleanup_test_files();

    t_ast_node *ast = fake_parse_input("echo test > test_multiple1.txt > test_multiple2.txt");
    cr_assert_not_null(ast, "AST should not be null");
    cr_assert_eq(ast->redir_count, 2, "Should have two redirections");

    for (int i = 0; i < ast->redir_count; i++)
    {
        cr_assert_eq(ast->redirs[i].type, REDIRECT_OUT, "Both should be output redirections");
    }
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}

Test(redirections, command_parsing_with_redirections)
{
    cleanup_test_files();

    t_ast_node *ast = fake_parse_input("echo hello world > output.txt");
    cr_assert_not_null(ast, "AST should not be null");

    cr_assert_not_null(ast->argv, "argv should not be null");
    cr_assert_str_eq(ast->argv[0], "echo", "First arg should be echo");
    cr_assert_str_eq(ast->argv[1], "hello", "Second arg should be hello");
    cr_assert_str_eq(ast->argv[2], "world", "Third arg should be world");
    cr_assert_null(ast->argv[3], "Fourth arg should be null");

    cr_assert_eq(ast->redir_count, 1, "Should have one redirection");
    cr_assert_eq(ast->redirs[0].type, REDIRECT_OUT, "Should be output redirection");
    cr_assert_str_eq(ast->redirs[0].file, "output.txt", "File should match");
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}

Test(redirections, syntax_error_no_filename)
{
    cleanup_test_files();

    t_ast_node *ast = fake_parse_input("echo test >");
    cr_assert_null(ast, "AST should be null for syntax error");
    
    cleanup_test_files();
}

Test(redirections, syntax_error_no_command)
{
    cleanup_test_files();
 
    t_ast_node *ast = fake_parse_input("> output.txt");
    cr_assert_null(ast, "AST should be null when no command is specified");
    
    cleanup_test_files();
}

Test(redirections, complex_redirection_parsing)
{
    cleanup_test_files();

    int fd = open("test_input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, "test content\n", 13);
    close(fd);

    t_ast_node *ast = fake_parse_input("sort < test_input.txt > sorted.txt 2> errors.txt");
    cr_assert_not_null(ast, "AST should not be null");
    cr_assert_eq(ast->redir_count, 3, "Should have three redirections");
    
    int input_found = 0, output_found = 0, stderr_found = 0;
    for (int i = 0; i < ast->redir_count; i++)
    {
        if (ast->redirs[i].type == REDIRECT_IN)
            input_found = 1;
        else if (ast->redirs[i].type == REDIRECT_OUT)
            output_found = 1;
        else if (ast->redirs[i].type == REDIRECT_STDERR)
            stderr_found = 1;
    }
    
    cr_assert(input_found, "Input redirection should be found");
    cr_assert(output_found, "Output redirection should be found");
    cr_assert(stderr_found, "Stderr redirection should be found");
    
    fake_free_ast_node(ast);
    cleanup_test_files();
}
