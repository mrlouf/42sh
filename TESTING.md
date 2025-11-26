# Testing Setup for 42sh

This project uses [Criterion](https://github.com/Snaipe/Criterion) for unit testing, providing a professional xUnit-style testing framework for C.

## Quick Start

```bash
make test
```

## What is Criterion?

Criterion is a modern, feature-rich unit testing framework for C and C++, designed to provide the same professional testing experience found in languages like Java (JUnit), Python (pytest), or JavaScript (Jest). It brings enterprise-grade testing capabilities to C development.

### Key Features

- **Automatic Test Discovery**: No manual test registration required
- **Rich Assertion Library**: Comprehensive set of assertion macros
- **Test Isolation**: Each test runs in a separate process for complete isolation
- **Output Redirection**: Capture and test stdout/stderr output
- **Parameterized Tests**: Run the same test with different data sets
- **Test Fixtures**: Setup and teardown functions for test preparation
- **Signal Handling**: Graceful handling of crashes and signals
- **Memory Safety**: Built-in memory leak detection capabilities
- **Reporting**: Multiple output formats (TAP, XML, JSON) for CI integration

### Why Criterion over alternatives?

| Feature | Criterion | Unity | CUnit | Manual Testing |
|---------|-----------|-------|-------|----------------|
| Auto Discovery | ✅ | ❌ | ❌ | ❌ |
| Process Isolation | ✅ | ❌ | ❌ | ❌ |
| Modern C99+ | ✅ | ✅ | ❌ | N/A |
| Rich Assertions | ✅ | ✅ | ⚠️ | ❌ |
| CI Integration | ✅ | ⚠️ | ⚠️ | ❌ |
| Memory Safety | ✅ | ❌ | ❌ | ❌ |

## How Criterion Works

### Test Discovery and Execution

1. **Compilation**: Test files are compiled with special Criterion sections
2. **Discovery**: Criterion scans the binary for test metadata at runtime
3. **Isolation**: Each test runs in a separate process (fork/exec)
4. **Reporting**: Results are collected and formatted for output

### Test Structure

```
TestSuite(suite_name) → Groups related tests
    ├── Test(suite_name, test_name) → Individual test case
    ├── Test(suite_name, another_test) → Another test case
    └── ...
```

### Process Isolation Benefits

- **Crash Safety**: One test crash doesn't affect others
- **Memory Isolation**: No shared memory corruption between tests
- **Signal Handling**: Tests can test signal-raising code safely
- **Resource Cleanup**: Automatic cleanup on test completion

## Professional Testing Standards for 42sh

### Test Organization

```
tests/
├── test_builtins_echo.c     # Echo command tests
├── test_builtins_cd.c       # CD command tests  
├── test_env_manager.c       # Environment management tests
├── test_executor.c          # Command execution tests
├── test_parser.c            # Input parsing tests
└── test_integration.c       # End-to-end integration tests
```

### Naming Conventions

- **Files**: `test_<module>_<component>.c`
- **Test Suites**: `<component>_<functionality>` (e.g., `echo_builtin`, `cd_builtin`)
- **Test Cases**: `<specific_behavior>` (e.g., `simple_text`, `suppress_newline`)

### Test Categories

#### 1. Unit Tests (Isolated Function Testing)

```c
// Test individual functions in isolation
Test(echo_builtin, handles_escape_sequences) {
    char *argv[] = {"echo", "-e", "hello\\nworld", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0);
    cr_assert_stdout_eq_str("hello\nworld\n");
}
```

#### 2. Integration Tests (Component Interaction)

```c
// Test how multiple components work together
Test(shell_integration, command_execution_flow) {
    t_shell shell = {0};
    init_shell(&shell, environ);
    
    // Test complete command execution pipeline
    int result = execute_command(&shell, "echo hello");
    
    cr_assert_eq(result, 0);
    // Additional assertions...
}
```

#### 3. Edge Case Tests (Boundary Conditions)

```c
// Test error conditions and edge cases
Test(echo_builtin, handles_empty_input) {
    char *argv[] = {"echo", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0);
    cr_assert_stdout_eq_str("\n");
}
```

### Writing Quality Tests

#### Test Structure (AAA Pattern)

```c
Test(suite_name, test_name) {
    // ARRANGE: Set up test data and conditions
    char *argv[] = {"echo", "-n", "hello", NULL};
    
    // ACT: Execute the function under test
    int result = builtin_echo(argv);
    
    // ASSERT: Verify the expected behavior
    cr_assert_eq(result, 0);
    cr_assert_stdout_eq_str("hello");
}
```

#### Essential Assertion Types

```c
// Value comparisons
cr_assert_eq(actual, expected, "Error message");
cr_assert_neq(actual, unexpected);
cr_assert_lt(actual, boundary);
cr_assert_gt(actual, boundary);

// String comparisons  
cr_assert_str_eq(actual, expected);
cr_assert_str_neq(actual, unexpected);

// Pointer checks
cr_assert_null(pointer);
cr_assert_not_null(pointer);

// Output verification
cr_assert_stdout_eq_str(expected_output);
cr_assert_stderr_eq_str(expected_error);

// Boolean conditions
cr_assert(condition, "Custom message");
```

#### Test Fixtures (Setup/Teardown)

```c
// Global setup for all tests in suite
void setup_shell_tests(void) {
    cr_redirect_stdout();
    cr_redirect_stderr();
    // Initialize common test data
}

void teardown_shell_tests(void) {
    // Cleanup test data
    cr_redirect_stdout(); // Reset redirection
    cr_redirect_stderr();
}

TestSuite(shell_execution, .init = setup_shell_tests, .fini = teardown_shell_tests);
```

#### Testing Output Capture

```c
Test(echo_builtin, captures_stdout_correctly) {
    // Criterion automatically captures stdout when cr_redirect_stdout() is called
    char *argv[] = {"echo", "test output", NULL};
    
    builtin_echo(argv);
    
    // Verify captured output
    cr_assert_stdout_eq_str("test output\n");
}
```

#### Parameterized Tests

```c
// Test data structure
struct echo_test_case {
    char **argv;
    char *expected_output;
    int expected_return;
};

// Test data
ParameterizedTestParameters(echo_builtin, escape_sequences) {
    static struct echo_test_case cases[] = {
        {(char*[]){"echo", "-e", "hello\\nworld", NULL}, "hello\nworld\n", 0},
        {(char*[]){"echo", "-e", "tab\\ttest", NULL}, "tab\ttest\n", 0},
        {(char*[]){"echo", "-e", "backslash\\\\", NULL}, "backslash\\\n", 0},
    };
    
    return cr_make_param_array(struct echo_test_case, cases, 
                              sizeof(cases) / sizeof(cases[0]));
}

ParameterizedTest(struct echo_test_case *case_data, echo_builtin, escape_sequences) {
    int result = builtin_echo(case_data->argv);
    
    cr_assert_eq(result, case_data->expected_return);
    cr_assert_stdout_eq_str(case_data->expected_output);
}
```

### Error Handling and Edge Cases

#### Testing Error Conditions

```c
Test(cd_builtin, handles_nonexistent_directory) {
    t_shell shell = {0};
    char *argv[] = {"cd", "/nonexistent/path", NULL};
    
    int result = builtin_cd(&shell, argv);
    
    cr_assert_neq(result, 0, "Should return error code");
    cr_assert_stderr_neq_str("", "Should output error message");
}
```

#### Testing Memory Safety

```c
Test(env_manager, no_memory_leaks) {
    t_shell shell = {0};
    init_shell(&shell, environ);
    
    // Perform operations that allocate memory
    set_environment_variable(&shell, "TEST_VAR", "test_value");
    
    // Cleanup should handle all allocations
    cleanup_shell(&shell);
    
    // Criterion + Valgrind will detect any leaks
}
```

### Test Documentation

#### Self-Documenting Tests

```c
Test(echo_builtin, posix_compliance_suppress_newline_flag) {
    // POSIX requirement: echo -n should suppress trailing newline
    char *argv[] = {"echo", "-n", "hello world", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0, "echo should return success");
    cr_assert_stdout_eq_str("hello world", "Should output text without newline");
}
```

#### Test Comments

```c
Test(parser, handles_complex_command_line) {
    /*
     * Test case: Complex command with pipes, redirections, and quotes
     * Input: `echo "hello world" | grep hello > output.txt`
     * Expected: Should parse into proper token structure
     */
    char *input = "echo \"hello world\" | grep hello > output.txt";
    
    t_token_list *tokens = parse_command_line(input);
    
    cr_assert_not_null(tokens);
    // Verify token structure...
}
```

## Installation Options

The Makefile automatically detects your Criterion installation and falls back appropriately:

### Option 1: System Package (Recommended for most users)

**Ubuntu/Debian:**
```bash
sudo apt-get install libcriterion-dev
```

**macOS:**
```bash
brew install criterion
```

**Arch Linux:**
```bash
sudo pacman -S criterion
```

### Option 2: Local Installation (For systems without sudo)

If you can't install system packages (like on campus computers), you can build Criterion locally:

```bash
# Install build dependencies
pip3 install --user meson ninja

# Clone and build Criterion
mkdir -p deps && cd deps
git clone --recursive https://github.com/Snaipe/Criterion.git
cd Criterion
export PATH=$HOME/.local/bin:$PATH
meson setup --prefix=$HOME/.local builddir
cd builddir && meson compile && meson install
```

The Makefile will automatically detect and use the local installation.

## Current Test Coverage

- **Echo builtin**: Tests POSIX compliance, escape sequences, flags (-n, -e)
- **CD builtin**: Tests directory navigation and error handling
- **Infrastructure**: Basic framework validation tests

## Writing New Tests

Create test files in the `tests/` directory following the pattern `test_*.c`:

```c
#include <criterion/criterion.h>
#include "../incs/42sh.h"

TestSuite(your_feature);

Test(your_feature, test_case_name) {
    // Your test code here
    cr_assert_eq(expected, actual, "Error message");
}
```

## CI Integration

Tests run automatically on GitHub Actions for:
- Ubuntu (latest)
- macOS (latest)
- Multiple compiler configurations

See `.github/workflows/42sh_CI.yml` for details.

## Testing Best Practices for 42sh

### Test-Driven Development (TDD)

1. **Red**: Write a failing test that describes the desired behavior
2. **Green**: Write minimal code to make the test pass
3. **Refactor**: Clean up code while keeping tests green

```c
// Example TDD cycle for new builtin command
Test(pwd_builtin, returns_current_directory) {
    // RED: This test will fail initially
    char *result = builtin_pwd();
    cr_assert_str_eq(result, getcwd(NULL, 0));
}
```

### Test Coverage Guidelines

- **Functions**: Every public function should have tests
- **Branches**: Test both success and error paths
- **Edge Cases**: Empty input, NULL pointers, boundary values
- **Integration**: Test component interactions

### Code Quality Standards

```c
// GOOD: Clear, focused test
Test(echo_builtin, suppresses_newline_with_n_flag) {
    char *argv[] = {"echo", "-n", "hello", NULL};
    
    int result = builtin_echo(argv);
    
    cr_assert_eq(result, 0);
    cr_assert_stdout_eq_str("hello");
}

// AVOID: Testing multiple behaviors in one test
Test(echo_builtin, all_flags_and_behaviors) {
    // This test is too broad and hard to debug
    // Split into separate focused tests instead
}
```

### Common Testing Patterns

#### Testing Shell State Changes

```c
Test(cd_builtin, updates_pwd_environment_variable) {
    t_shell shell = {0};
    init_shell(&shell, environ);
    char *original_pwd = get_env_value(&shell, "PWD");
    char *argv[] = {"cd", "/tmp", NULL};
    
    builtin_cd(&shell, argv);
    
    char *new_pwd = get_env_value(&shell, "PWD");
    cr_assert_str_neq(original_pwd, new_pwd);
    cr_assert_str_eq(new_pwd, "/tmp");
    
    cleanup_shell(&shell);
}
```

#### Testing Error Messages

```c
Test(builtin_manager, reports_unknown_command) {
    t_shell shell = {0};
    char *argv[] = {"nonexistent_command", NULL};
    
    int result = execute_builtin(&shell, argv);
    
    cr_assert_eq(result, COMMAND_NOT_FOUND);
    cr_assert_stderr_neq_str("", "Should output error message");
}
```

#### Testing Signal Handling

```c
Test(executor, handles_sigint_gracefully) {
    // Criterion's process isolation makes signal testing safe
    t_shell shell = {0};
    init_signal_handlers(&shell);
    
    // Simulate SIGINT
    kill(getpid(), SIGINT);
    
    // Verify shell state after signal
    cr_assert_eq(shell.last_exit_status, 130); // SIGINT exit code
}
```

### Performance Testing

```c
Test(parser, performance_large_input) {
    // Create large input string (e.g., 1MB command line)
    char *large_input = create_large_test_input(1024 * 1024);
    
    clock_t start = clock();
    t_token_list *tokens = parse_command_line(large_input);
    clock_t end = clock();
    
    double cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    cr_assert_not_null(tokens);
    cr_assert_lt(cpu_time, 1.0, "Should parse 1MB input in under 1 second");
    
    free(large_input);
    free_token_list(tokens);
}
```

### Debugging Test Failures

#### Running Specific Tests

```bash
# Run only tests from specific suite
./test_runner --filter='echo_builtin/*'

# Run specific test
./test_runner --filter='echo_builtin::simple_text'

# Debug mode with verbose output
./test_runner --verbose --debug
```

#### GDB Integration

```bash
# Debug failing test with GDB
gdb --args ./test_runner --filter='problematic_test'
(gdb) run
(gdb) bt  # When test fails, get backtrace
```

### Memory Testing Integration

```bash
# Run tests with Valgrind
valgrind --tool=memcheck --leak-check=full ./test_runner

# Or use the built-in memory testing
./test_runner --always-succeed --verbose 2>&1 | grep -i "memory"
```

## Continuous Integration Testing

Our GitHub Actions workflow automatically:

1. **Builds**: Compiles project with multiple compilers
2. **Tests**: Runs full test suite with Criterion
3. **Memory Check**: Validates with Valgrind  
4. **Coverage**: Generates code coverage reports
5. **Artifacts**: Saves test results and coverage data

### Local CI Simulation

```bash
# Simulate CI environment locally
make clean
make CFLAGS="-Wall -Wextra -Werror -O2" test
valgrind --tool=memcheck ./test_runner
```

## Contributing New Tests

When adding new functionality to 42sh:

1. **Write tests first** (TDD approach)
2. **Follow naming conventions** (suite_functionality, specific_behavior)
3. **Include edge cases** (NULL inputs, memory limits, error conditions)
4. **Document complex tests** (comments explaining the test purpose)
5. **Run full test suite** (ensure no regressions)

Example contribution workflow:

```c
// 1. Add test for new feature
Test(type_builtin, identifies_builtin_commands) {
    char *argv[] = {"type", "echo", NULL};
    
    int result = builtin_type(argv);
    
    cr_assert_eq(result, 0);
    cr_assert_stdout_eq_str("echo is a shell builtin\n");
}

// 2. Implement feature to make test pass
// 3. Add edge case tests
// 4. Refactor and optimize
// 5. Verify full suite passes
```

This comprehensive testing approach ensures our 42sh implementation is robust, maintainable, and meets professional software development standards.
