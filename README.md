# 42sh

A POSIX-compliant shell implementation for the 42 school curriculum.

## Building

```bash
make
```

## Testing

This project includes comprehensive unit tests using the Criterion framework.

```bash
# Run tests (will check for Criterion automatically)
make test

# If Criterion is not installed, set it up locally:
make test-setup
```

For detailed testing documentation, see [TESTING.md](TESTING.md).

## Features

- POSIX-compliant builtin commands (echo, cd, env, exit, type)
- Command execution and job control
- Environment variable management
- Comprehensive test coverage

## Development

The project follows professional development practices with:
- Unit testing with Criterion framework
- Continuous Integration via GitHub Actions
- Memory leak detection with Valgrind
- Strict compiler warnings and code standards 
