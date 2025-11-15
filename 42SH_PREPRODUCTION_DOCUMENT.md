# 42sh Pre-Production Document
## "ONE FOR THE AGES" - A Comprehensive Strategy Guide

**Team Members:**
- **Nico** - Parser Lead (Minishell Parser Experience)
- **Eva** - Parser Developer (Minishell Parser Experience)
- **Hugo** - Executor Lead (Minishell Executor Experience)
- **Marc** - Executor Developer (Minishell Executor Experience)

**Project Start Date:** November 15, 2025  
**Target Completion:** TBD (Recommend 3-4 month timeline)

---

## Table of Contents
1. [Project Philosophy & Core Principles](#1-project-philosophy--core-principles)
2. [Reference Shell Selection](#2-reference-shell-selection)
3. [Architecture Overview](#3-architecture-overview)
4. [Team Structure & Responsibilities](#4-team-structure--responsibilities)
5. [Development Goals & Roadmap](#5-development-goals--roadmap)
6. [Feature Breakdown & Priority](#6-feature-breakdown--priority)
7. [Technical Specifications](#7-technical-specifications)
8. [Testing Strategy](#8-testing-strategy)
9. [Risk Management](#9-risk-management)
10. [Code Quality Guidelines](#10-code-quality-guidelines)

---

## 1. Project Philosophy & Core Principles

### 1.1 The Golden Rule: STABILITY OVER FEATURES
> "A humble and indestructible 42sh is always better than a kooky 42sh with countless features that segfault in unanticipated ways."

**Core Principles:**
- ✅ **Stability First:** Every feature must be rock-solid before moving to the next
- ✅ **Incremental Progress:** Build in layers, test extensively at each stage
- ✅ **Team Sanity:** Sustainable development pace, no burnout
- ✅ **POSIX Compliance:** When in doubt, follow POSIX standards
- ✅ **User Experience:** Intuitive for bash/sh users

### 1.2 Project Goals
**PRIMARY GOAL:** Pass the project with a stable, well-featured shell  
**SECONDARY GOAL:** Build something we're proud of  
**ANTI-GOAL:** Don't sacrifice stability for feature count

---

## 2. Reference Shell Selection

### 2.1 Recommended Approach: **bash** as Primary Reference
**Rationale:**
- More feature-complete than `sh` but less complex than `zsh`
- Well-documented behavior
- Team likely has extensive experience using it
- POSIX-compliant with useful extensions
- Excellent middle ground for learning

### 2.2 Testing Protocol
**Compare behavior against:**
1. **bash** (primary reference)
2. **sh** (POSIX baseline)
3. **zsh** (optional - for edge cases)

**Method:**
```bash
# Always test in parallel
bash -c "command" > bash_output.txt 2>&1
./42sh -c "command" > 42sh_output.txt 2>&1
diff bash_output.txt 42sh_output.txt
```

---

## 3. Architecture Overview

### 3.1 High-Level Component Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                         42sh SHELL                           │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────┐      ┌──────────────┐      ┌───────────┐ │
│  │   READLINE   │──────▶│    PARSER    │──────▶│  EXECUTOR │ │
│  │   (Input)    │      │   (AST Gen)   │      │  (Runner) │ │
│  └──────────────┘      └──────────────┘      └───────────┘ │
│         │                     │                      │       │
│         │                     │                      │       │
│  ┌──────▼──────┐      ┌──────▼──────┐      ┌────────▼────┐ │
│  │   SIGNAL    │      │   LEXER/    │      │   BUILTINS  │ │
│  │   HANDLER   │      │  TOKENIZER  │      │   EXECUTOR  │ │
│  └─────────────┘      └─────────────┘      └─────────────┘ │
│                              │                      │       │
│                       ┌──────▼──────┐      ┌────────▼────┐ │
│                       │  EXPANSION  │      │ JOB CONTROL │ │
│                       │   ENGINE    │      │   MANAGER   │ │
│                       └─────────────┘      └─────────────┘ │
│                                                     │       │
│  ┌──────────────────────────────────────────────────▼────┐ │
│  │           ENVIRONMENT & VARIABLE MANAGER              │ │
│  └───────────────────────────────────────────────────────┘ │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Data Flow

```
User Input
    ↓
[Line Editing & History] (readline/custom)
    ↓
[Tokenizer/Lexer] → Tokens
    ↓
[Parser] → Abstract Syntax Tree (AST)
    ↓
[Expansion Engine] → Expanded AST
    ↓
[Executor] → Process Management
    ↓
[Job Control] → Background/Foreground Management
    ↓
Exit Status & Output
```

---

## 4. Team Structure & Responsibilities

### 4.1 Team Organization

#### 🔵 **PARSER TEAM** (Nico & Eva)
**Primary Responsibility:** Input → AST Generation

**Core Modules:**
1. **Lexer/Tokenizer**
   - Input sanitization
   - Token recognition (operators, words, quotes, etc.)
   - Quote handling (initial implementation)

2. **Parser/AST Builder**
   - Grammar implementation
   - AST construction
   - Syntax validation
   - Operator precedence

3. **Expansion Engine** (Shared responsibility)
   - Variable expansion (`$VAR`, `${VAR}`)
   - Command substitution (`$(...)`)
   - Tilde expansion (`~`)
   - Advanced expansions (modular)
   - Globbing/Pattern matching (modular)

**Key Deliverables:**
- Token stream generator
- AST data structures
- Parser logic
- Expansion utilities

---

#### 🟢 **EXECUTOR TEAM** (Hugo & Marc)
**Primary Responsibility:** AST → Process Execution

**Core Modules:**
1. **Execution Engine**
   - Command execution (external programs)
   - Pipeline management
   - Redirection handling
   - Operator execution (`;`, `&&`, `||`)

2. **Built-in Commands**
   - `cd`, `echo`, `exit`, `type`
   - `export`, `unset`, `set`
   - `jobs`, `fg`, `bg`
   - Additional modular built-ins

3. **Job Control System**
   - Process group management
   - Background/foreground control
   - Signal handling (SIGINT, SIGTSTP, etc.)

4. **Environment Management**
   - Variable storage (internal vs environment)
   - Export/unexport logic
   - Inheritance for child processes

**Key Deliverables:**
- Execution dispatcher
- Built-in command handlers
- Job control system
- Environment variable manager

---

### 4.2 Shared Responsibilities (All Team Members)

#### 🟡 **INFRASTRUCTURE** (Rotating ownership)
- **Input/Line Editing:** Basic readline integration → Custom line editing
- **Signal Handling:** Proper signal setup and handling
- **Error Management:** Unified error reporting system
- **Memory Management:** Leak prevention, cleanup routines
- **Testing Framework:** Unit tests, integration tests

#### 🟣 **CROSS-CUTTING CONCERNS**
- **Data Structures:** Shared structs for AST, tokens, jobs, etc.
- **Utilities:** String manipulation, path resolution, etc.
- **Documentation:** Code comments, README, man pages

---

### 4.3 Collaboration Model

```
PARSER TEAM                    EXECUTOR TEAM
     │                              │
     │  AST Interface Definition    │
     ├──────────────────────────────┤
     │                              │
     │  Expansion Coordination      │
     ├──────────────────────────────┤
     │                              │
     │  Variable/Env Integration    │
     └──────────────────────────────┘
```

**Communication Protocol:**
- **Daily Standups:** Quick sync on progress, blockers (15 min)
- **Weekly Integration:** Merge parser + executor work
- **Pair Programming:** Cross-team for complex features
- **Code Reviews:** Mandatory for all PRs

---

## 5. Development Goals & Roadmap

### 5.1 Goal-Based Development Strategy

Instead of rigid time-based schedules, we organize work into goal-oriented phases called **develop0** through **develop5**. Each phase corresponds to a Git branch and represents a complete, stable milestone. Only move to the next phase when the current one is **rock-solid**.

| Phase | Branch | Focus | Completion Criteria |
|-------|--------|-------|---------------------|
| **develop0** | `develop0` | Setup & Research | Project structure ready, team aligned |
| **develop1** | `develop1` | MVP Mandatory | Basic shell executes commands |
| **develop2** | `develop2` | Complete Mandatory | All mandatory features stable |
| **develop3** | `develop3` | Modular Features | 6+ modular features implemented |
| **develop4** | `develop4` | Polish & Testing | Production ready, defense-ready |
| **develop5** | `develop5` | Bonus (Optional) | Extra features (only if develop4 perfect) |

---

### 5.2 Detailed Goal Breakdown

## **DEVELOP0: Foundation & Setup**
**Git Branch:** `develop0`

**🎯 Goals:**
- Project structure established
- Development environment configured
- Research completed
- Team aligned on architecture

**📋 Tasks:**

**All Team:**
- [ ] Set up Git repository with proper `.gitignore`
- [ ] Create project directory structure
- [ ] Set up libft integration
- [ ] Create Makefile skeleton
- [ ] Research POSIX shell standards
- [ ] Test and compare `bash`, `sh`, `zsh` behaviors
- [ ] Define shared data structures
- [ ] Establish coding standards

**Parser Team:**
- [ ] Research shell grammar (BNF notation)
- [ ] Study tokenization patterns
- [ ] Design AST node structures
- [ ] Plan expansion engine architecture

**Executor Team:**
- [ ] Research `fork()`, `execve()`, `wait()` patterns
- [ ] Study job control (`setpgid()`, `tcsetpgrp()`)
- [ ] Plan signal handling strategy
- [ ] Design built-in command interface

**Deliverables:**
- ✅ Compiled skeleton that runs
- ✅ Architecture document (this document!)
- ✅ Shared header files with basic structs
- ✅ Test script framework

---

## **DEVELOP1: MVP Mandatory**
**Git Branch:** `develop1`

**🎯 Goals:**
- Minimal working shell
- Can execute simple commands
- Basic error handling
- Foundation for expansion

**📋 Features to Implement:**

### Goal 1.1: Basic Command Execution

**Parser Team:**
```
Priority 1: Simple Tokenizer
- Split input by spaces
- Recognize basic tokens (words, operators)
- Handle simple quotes (basic)

Priority 2: Minimal Parser
- Parse single commands (no pipes/redirections yet)
- Build simple AST nodes
- Handle command + arguments
```

**Executor Team:**
```
Priority 1: Simple Executor
- Execute external commands via execve()
- PATH resolution
- Basic fork/wait pattern

Priority 2: Environment Setup
- Copy environment on startup
- Pass environment to child processes
- Basic variable storage
```

**Integration Point:**
```c
// Example interface
typedef struct s_ast_node {
    int type; // COMMAND, PIPE, REDIRECT, etc.
    char **argv; // For commands
    struct s_ast_node *left;
    struct s_ast_node *right;
} t_ast_node;

// Parser delivers AST to executor
t_ast_node *parse_input(char *input);
int execute_ast(t_ast_node *ast);
```

**Test Cases:**
```bash
# Should work by end of Goal 1.1
42sh$ ls
42sh$ ls -la
42sh$ /bin/echo hello
42sh$ pwd
```

---

### Goal 1.2: Built-ins & Operators

**Parser Team:**
```
Priority 1: Operator Recognition
- Tokenize pipes |
- Tokenize semicolons ;
- Build AST with operator nodes
- Handle operator precedence (basic)

Priority 2: Enhanced Tokenizer
- Better quote handling
- Escape sequences (basic)
```

**Executor Team:**
```
Priority 1: Core Built-ins
- Implement cd (basic - just directory change)
- Implement echo (no options yet)
- Implement exit
- Implement type (command type identification)

Priority 2: Pipeline Execution
- Set up pipe() between commands
- Handle multiple pipes
- Wait for all processes
```

**Test Cases:**
```bash
42sh$ cd /tmp
42sh$ pwd
/tmp
42sh$ echo hello world
hello world
42sh$ ls | grep test
42sh$ echo a ; echo b
a
b
```

---

### Goal 1.3: Redirections & Logical Operators

**Parser Team:**
```
Priority 1: Redirection Parsing
- Parse >, >>, <, <<
- Parse >&, <&
- Add redirection info to AST

Priority 2: Logical Operators
- Parse && and ||
- Proper precedence handling
- Build conditional execution nodes
```

**Executor Team:**
```
Priority 1: Redirection Execution
- Implement >, >>, <
- Implement here-doc <<
- Implement >&, <& (file descriptor redirection)
- Handle redirection order/precedence

Priority 2: Logical Execution
- Execute && (short-circuit on failure)
- Execute || (short-circuit on success)
- Track and use exit status
```

**Test Cases:**
```bash
42sh$ echo hello > file.txt
42sh$ cat < file.txt
hello
42sh$ echo test >> file.txt
42sh$ cat << EOF
> line1
> line2
> EOF
42sh$ ls /invalid 2>&1 | grep "No such file"
42sh$ echo a && echo b
a
b
42sh$ false || echo "failed as expected"
failed as expected
```

**End of DEVELOP1 Milestone:**
- ✅ Basic shell that executes commands
- ✅ Pipes and redirections work
- ✅ Logical operators function correctly
- ✅ Core built-ins implemented
- ✅ No memory leaks in basic operations
- ✅ No segfaults on valid input

---

## **DEVELOP2: Complete Mandatory**
**Git Branch:** `develop2`

**🎯 Goals:**
- All mandatory features implemented
- Robust error handling
- Signal handling correct
- Job control functional

### Goal 2.1: Variables & Expansions

**Parser Team:**
```
Priority 1: Variable Expansion
- Implement $VAR expansion
- Implement ${VAR} expansion
- Implement ${?} for exit status
- Handle expansion in quotes correctly

Priority 2: Assignment Parsing
- Parse name=value syntax
- Handle VAR=value command syntax
- Distinguish assignment from command
```

**Executor Team:**
```
Priority 1: Variable Management
- Internal variable storage (hash table recommended)
- Environment vs internal variable distinction
- Implement export built-in
- Implement unset built-in
- Implement set built-in (list variables)

Priority 2: Variable Inheritance
- Per-command environment (HOME=/tmp cd)
- Proper variable scoping
```

**Test Cases:**
```bash
42sh$ VAR=hello
42sh$ echo $VAR
hello
42sh$ echo ${VAR}
hello
42sh$ export VAR
42sh$ env | grep VAR
VAR=hello
42sh$ echo ${?}
0
42sh$ false
42sh$ echo ${?}
1
42sh$ HOME=/tmp cd && pwd
/tmp
42sh$ unset VAR
42sh$ echo $VAR

42sh$ set
# Should list all internal variables
```

---

### Goal 2.2: Job Control Foundation

**Parser Team:**
```
Priority 1: Background Operator
- Parse & operator
- Add to AST appropriately

Priority 2: Support Executor
- Ensure AST supports job metadata
- Coordinate with executor on job structures
```

**Executor Team:**
```
Priority 1: Job Control System
- Implement job list structure
- Process group management (setpgid)
- Terminal control (tcsetpgrp)
- Background execution (&)

Priority 2: Job Built-ins
- Implement jobs (list jobs)
- Implement fg (foreground job)
- Implement bg (background job)
- Job numbering and tracking
```

**Test Cases:**
```bash
42sh$ sleep 100 &
[1] 12345
42sh$ jobs
[1]+  Running    sleep 100 &
42sh$ fg %1
# Brings sleep to foreground
^Z
[1]+  Stopped    sleep 100
42sh$ bg %1
[1]+ sleep 100 &
42sh$ jobs
[1]+  Running    sleep 100 &
```

---

### Goal 2.3: Signal Handling & Hardening

**All Team:**
```
Priority 1: Signal Handling
- Handle SIGINT (Ctrl+C) correctly
  - In shell: don't exit, new prompt
  - In foreground command: terminate command
- Handle SIGTSTP (Ctrl+Z) correctly
  - Suspend foreground job
- Handle SIGCHLD (reap zombies)
- Ignore SIGTTOU, SIGTTIN appropriately

Priority 2: Error Hardening
- Comprehensive error messages
- Graceful handling of edge cases
- No memory leaks in all code paths
- No segfaults on malformed input
```

**Parser Team:**
```
- Handle syntax errors gracefully
- Report clear error messages
- Handle unclosed quotes, unmatched operators
- Handle extremely long input
```

**Executor Team:**
```
- Handle command not found
- Handle permission denied
- Handle broken pipes
- Handle invalid redirections (e.g., >/)
- Handle too many open files
```

**Test Cases:**
```bash
42sh$ ^C
42sh$ # Should not exit, just new prompt
42sh$ cat
# Type something
^C
42sh$ # Should interrupt cat, not shell
42sh$ sleep 100
^Z
[1]+  Stopped    sleep 100
42sh$ # Should stop job, return to prompt

# Error cases
42sh$ cat < /nonexistent
42sh: /nonexistent: No such file or directory
42sh$ /bin/nonexistent
42sh: /bin/nonexistent: No such file or directory
42sh$ echo "unclosed
42sh: syntax error: unclosed quote
```

**End of DEVELOP2 Milestone:**
- ✅ **ALL MANDATORY FEATURES IMPLEMENTED**
- ✅ Signal handling perfect
- ✅ Job control functional
- ✅ Variables and expansions working
- ✅ Comprehensive error handling
- ✅ **ZERO MEMORY LEAKS**
- ✅ **ZERO SEGFAULTS**
- ✅ Extensive test suite passing

**🚨 CHECKPOINT: Do NOT proceed to DEVELOP3 until DEVELOP2 is rock-solid!**

---

## **DEVELOP3: Modular Features**
**Git Branch:** `develop3`

**🎯 Goals:**
- Implement minimum 6 modular features
- Maintain stability (test after each feature!)
- Choose features strategically

### 5.3 Recommended Modular Features (Prioritized)

**Tier 1 - High Value, Moderate Complexity (IMPLEMENT THESE)**

1. **✅ Inhibitors (Quotes & Backslash)** - Goal 3.1
   - Parser Team Lead
   - Essential for practical use
   - Already partially implemented
   - **Complexity: Medium**

2. **✅ Pattern Matching (Globbing)** - Goal 3.1
   - Parser Team Lead
   - High user value
   - Can reuse existing code patterns
   - **Complexity: Medium**

3. **✅ Command Substitution `$()`** - Goal 3.2
   - Parser & Executor collaboration
   - Very useful feature
   - Builds on existing pipe knowledge
   - **Complexity: Medium-High**

4. **✅ Tilde Expansion `~`** - Goal 3.2
   - Parser Team
   - Simple to implement
   - High value for users
   - **Complexity: Low**

5. **✅ History Management** - Goal 3.3
   - All Team
   - Using readline makes this easier
   - Good user experience
   - **Complexity: Medium**
   - Includes: `!!`, `!word`, `!number`, `!-number`, save to file, CTRL-R

6. **✅ Hash Table & `hash` built-in** - Goal 3.3
   - Executor Team
   - Performance optimization
   - Interesting feature
   - **Complexity: Low-Medium**

---

**Tier 2 - High Value, High Complexity (If time permits)**

7. **Arithmetic Expansion `$(())`** - Goal 3.4 (Optional)
   - Parser Team Lead
   - Requires expression parser
   - **Complexity: High**

8. **`test` built-in with operators** - Goal 3.4 (Optional)
   - Executor Team
   - Requires many file checks
   - **Complexity: Medium**

---

**Tier 3 - Lower Priority (Skip unless time)**

9. Advanced Parameter Expansions (`${parameter:-word}`, etc.)
   - **Complexity: Medium-High**
   - Less frequently used

10. Vi/Readline editing modes
    - **Complexity: Very High**
    - Time-consuming

11. Process substitution `<()` `>()`
    - **Complexity: Very High**
    - Requires advanced process management

12. Contextual completion
    - **Complexity: Very High**
    - Nice to have but not essential

---

### Goal 3.1: Quotes & Globbing

**Parser Team:**
```
Priority 1: Complete Quote Handling
- Single quotes (inhibit all expansion)
- Double quotes (allow some expansion)
- Backslash escaping
- Nested quote handling

Priority 2: Globbing Engine
- Implement * (match any)
- Implement ? (match one)
- Implement [] (character class)
- Implement ! (negation)
- Implement ranges with \
- Integrate with expansion engine
```

**Executor Team:**
```
- Support parser with testing
- Ensure executor handles expanded globs
- Fix any integration issues
```

**Test Cases:**
```bash
42sh$ echo 'test $VAR'
test $VAR
42sh$ echo "test $VAR"
test hello
42sh$ echo "test \"nested\""
test "nested"
42sh$ ls *.c
# Lists all .c files
42sh$ ls test?.c
# Lists test1.c, test2.c, etc.
42sh$ ls [a-z]*.c
# Lists files starting with lowercase letter
```

---

### Goal 3.2: Substitutions & Tilde

**Parser Team:**
```
Priority 1: Tilde Expansion
- Expand ~ to $HOME
- Handle ~username (optional)

Priority 2: Command Substitution
- Parse $() syntax
- Integrate with executor for nested execution
```

**Executor Team:**
```
Priority 1: Command Substitution Execution
- Execute command in subshell
- Capture stdout
- Replace in command line
- Handle nested substitutions
```

**Test Cases:**
```bash
42sh$ cd ~
42sh$ pwd
/home/username
42sh$ echo $(ls)
file1 file2 file3
42sh$ echo "Today is $(date)"
Today is Fri Nov 15 ...
42sh$ echo $(echo $(echo nested))
nested
```

---

### Goal 3.3: History & Hash Table

**All Team:**
```
Priority 1: History Management
- Implement !! (repeat last command)
- Implement !number (repeat command N)
- Implement !-number (repeat N commands ago)
- Implement !word (repeat last command starting with word)
- Save history to file (~/.42sh_history)
- Load history on startup
- CTRL-R incremental search (if using readline, mostly free)

Priority 2: Hash Table
- Implement command path caching
- Implement hash built-in
  - hash (show table)
  - hash -r (clear table)
  - hash command (add to table)
- Integrate with command execution
```

**Test Cases:**
```bash
42sh$ echo hello
hello
42sh$ !!
echo hello
hello
42sh$ echo test
test
42sh$ !1
echo hello
hello
42sh$ !-1
echo test
test
42sh$ !ec
echo test
test
# After restart
42sh$ # History persists
42sh$ hash
# Shows cached command paths
42sh$ hash ls
42sh$ hash
hits    command
  1     /bin/ls
```

---

**End of DEVELOP3 Milestone:**
- ✅ Minimum 6 modular features working
- ✅ All features tested thoroughly
- ✅ **STABILITY MAINTAINED** (no regressions)
- ✅ Still zero memory leaks
- ✅ Still zero segfaults

---

## **DEVELOP4: Polish & Testing**
**Git Branch:** `develop4`

**🎯 Goals:**
- Comprehensive testing
- Fix all bugs
- Performance optimization
- Documentation
- Defense preparation

**📋 Tasks:**

**All Team:**
```
DEVELOP4 Activities:

□ Comprehensive Test Suite
  - Test all mandatory features
  - Test all modular features
  - Test error cases
  - Test edge cases
  - Memory leak testing (valgrind)
  - Stress testing

□ Bug Fixing
  - Fix all known bugs
  - Handle all TODOs
  - Code cleanup

□ Performance Optimization
  - Profile with valgrind/gprof
  - Optimize hot paths
  - Ensure hash table helps

□ Documentation
  - Clean up code comments
  - Write comprehensive README
  - Document all features
  - Create usage examples

□ Defense Preparation
  - Practice explaining code
  - Prepare demo script
  - Know your features inside-out
  - Be ready for edge case questions
```

**Quality Checklist:**
```
□ Makefile works (all, clean, fclean, re)
□ Compiles with -Wall -Wextra -Werror
□ No compilation warnings
□ Executable named 42sh
□ libft compiles and links correctly
□ No memory leaks (valgrind --leak-check=full)
□ No segfaults on any input
□ Proper error messages
□ Signals handled correctly
□ All mandatory features work
□ 6+ modular features work
□ Code is readable and maintainable
□ Git history is clean
□ README is comprehensive
```

---

## **DEVELOP5: Bonus (Optional)**
**Git Branch:** `develop5`

**⚠️ ONLY IF DEVELOP4 IS PERFECT!**

**Recommended Bonuses:**

1. **Shell Scripting Support**
   - `if/then/else/fi`
   - `while/do/done`
   - `for/in/do/done`
   - `case/esac`
   - Functions

2. **Arithmetic Expansion** (if not done in Phase 3)

3. **Enhanced Completion**
   - Parameter completion for built-ins

**Code Quality Requirements for Bonuses:**
```
□ No excessive ternary operators
□ Explicit function names (no ft_parse1, ft_parse2)
□ Explicit variable names
□ Proper use of const qualifier
□ Clean git history with meaningful commits
□ Automated test suite
```

---

## 6. Feature Breakdown & Priority

### 6.1 Mandatory Features Checklist

**Basic Shell (Minishell Prerequisites):**
- [ ] Prompt display
- [ ] Execute commands with parameters
- [ ] PATH monitoring and resolution
- [ ] Proper space and tab handling

**Advanced Shell (42sh Prerequisites):**
- [ ] Full command line editing (readline or custom)
- [ ] Redirections: `>`, `>>`, `<`, `<<`, `>&`, `<&`
- [ ] Pipe `|`
- [ ] Separator `;`
- [ ] Logical operators `&&`, `||`

**Built-ins:**
- [ ] `cd` (with POSIX options)
- [ ] `echo` (with POSIX options)
- [ ] `exit`
- [ ] `type`
- [ ] `export`
- [ ] `set`
- [ ] `unset`

**Variables:**
- [ ] Internal variable creation (`name=value`)
- [ ] Variable export to environment
- [ ] List variables (`set`)
- [ ] Remove variables (`unset`)
- [ ] Per-command environment (`VAR=value command`)
- [ ] Simple expansion `${}`, `$VAR`
- [ ] Exit status `${?}`

**Job Control:**
- [ ] Background operator `&`
- [ ] Built-ins: `jobs`, `fg`, `bg`
- [ ] Proper process group management
- [ ] Terminal control

**Signal Handling:**
- [ ] SIGINT (Ctrl+C)
- [ ] SIGTSTP (Ctrl+Z)
- [ ] SIGCHLD
- [ ] Other relevant signals

**Operator Precedence:**
- [ ] Correct precedence of `;`, `&&`, `||`
- [ ] Correct precedence of redirections
- [ ] Proper execution order

---

### 6.2 Modular Features Selection (Choose 6+)

**Our Recommended Set (Strategic Selection):**

| # | Feature | Team Lead | Complexity | Value | Status |
|---|---------|-----------|------------|-------|--------|
| 1 | Quotes & Backslash | Parser | Medium | High | ⬜ |
| 2 | Globbing | Parser | Medium | High | ⬜ |
| 3 | Command Substitution `$()` | Both | Medium-High | High | ⬜ |
| 4 | Tilde `~` | Parser | Low | High | ⬜ |
| 5 | History Management | All | Medium | High | ⬜ |
| 6 | Hash Table & `hash` | Executor | Low-Medium | Medium | ⬜ |
| 7 | Arithmetic `$(())` | Parser | High | Medium | ⬜ |
| 8 | `test` built-in | Executor | Medium | Medium | ⬜ |

**Features to Avoid (Time Sink):**
- ❌ Vi/Readline modes (unless very confident)
- ❌ Process substitution `<()` `>()`
- ❌ Contextual completion (very complex)
- ❌ All parameter expansion formats (too many)

---

## 7. Technical Specifications

### 7.1 Recommended Data Structures

#### **Token Structure**
```c
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
```

#### **AST Node Structure**
```c
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
    int type; // <, >, >>, <<, <&, >&
    char *file; // filename or fd number
    int fd; // which fd to redirect
} t_redir;

typedef struct s_ast_node {
    t_ast_type type;
    
    // For commands
    char **argv;
    t_redir *redirs;
    int redir_count;
    
    // For operators
    struct s_ast_node *left;
    struct s_ast_node *right;
    
    // Metadata
    int background;
} t_ast_node;
```

#### **Job Structure**
```c
typedef enum e_job_status {
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} t_job_status;

typedef struct s_job {
    int job_id;
    pid_t pgid; // process group id
    char *command; // command string for display
    t_job_status status;
    int background;
    struct s_job *next;
} t_job;
```

#### **Variable Structure**
```c
typedef struct s_var {
    char *name;
    char *value;
    int exported; // 1 if in environment, 0 if internal only
    struct s_var *next;
} t_var;

// Or use hash table for better performance
#define VAR_HASH_SIZE 256

typedef struct s_var_table {
    t_var *buckets[VAR_HASH_SIZE];
    char **envp; // synchronized environment array
} t_var_table;
```

#### **Shell State**
```c
typedef struct s_shell {
    // Environment
    t_var_table *vars;
    
    // Job control
    t_job *jobs;
    int next_job_id;
    pid_t shell_pgid;
    int shell_terminal;
    int interactive;
    
    // History
    char *history_file;
    
    // Hash table for commands
    t_hash_table *cmd_cache;
    
    // Last exit status
    int last_exit_status;
    
    // Signal handling
    struct sigaction old_sigint;
    struct sigaction old_sigtstp;
    
} t_shell;
```

---

### 7.2 Module Interfaces

#### **Lexer Interface** (Parser Team)
```c
// Input: raw string
// Output: linked list of tokens
t_token *tokenize(char *input);
void free_tokens(t_token *tokens);
```

#### **Parser Interface** (Parser Team)
```c
// Input: token list
// Output: AST
t_ast_node *parse(t_token *tokens);
void free_ast(t_ast_node *ast);
```

#### **Expansion Interface** (Parser Team, used by Executor)
```c
// Expand variables, globs, etc.
char **expand_words(char **words, t_shell *shell);
char *expand_string(char *str, t_shell *shell);
```

#### **Executor Interface** (Executor Team)
```c
// Input: AST
// Output: exit status
int execute_ast(t_ast_node *ast, t_shell *shell);
```

#### **Built-in Interface** (Executor Team)
```c
typedef int (*t_builtin_func)(char **argv, t_shell *shell);

typedef struct s_builtin {
    char *name;
    t_builtin_func func;
} t_builtin;

// Built-in implementations
int builtin_cd(char **argv, t_shell *shell);
int builtin_echo(char **argv, t_shell *shell);
int builtin_exit(char **argv, t_shell *shell);
// ... etc
```

---

### 7.3 Key Algorithms

#### **Command Execution Flow**
```
1. Fork process
2. Setup redirections (dup2)
3. Setup process group (setpgid)
4. Give terminal control if foreground (tcsetpgrp)
5. Execute command (execve)
6. Parent: wait if foreground, add to job list if background
```

#### **Pipeline Execution**
```
For pipeline: cmd1 | cmd2 | cmd3

1. Create pipes: pipe1, pipe2
2. Fork cmd1:
   - stdout -> pipe1[write]
3. Fork cmd2:
   - stdin -> pipe1[read]
   - stdout -> pipe2[write]
4. Fork cmd3:
   - stdin -> pipe2[read]
5. Close all pipe fds in parent
6. Wait for all children
```

#### **Variable Expansion**
```
For each word in command:
  While word contains unescaped $:
    1. Find variable name
    2. Look up in variable table
    3. Replace with value
    4. Handle quotes correctly
  Return expanded word
```

---

## 8. Testing Strategy

### 8.1 Test-Driven Development Approach

**Philosophy:** Write tests as you implement features

**Test Categories:**
1. **Unit Tests** - Individual functions
2. **Integration Tests** - Component interactions
3. **Functional Tests** - End-to-end feature tests
4. **Regression Tests** - Previously fixed bugs
5. **Stress Tests** - Edge cases, large inputs

---

### 8.2 Testing Framework

**Recommended Structure:**
```
tests/
├── unit/
│   ├── test_lexer.c
│   ├── test_parser.c
│   ├── test_expansion.c
│   └── test_executor.c
├── integration/
│   ├── test_pipe.sh
│   ├── test_redirection.sh
│   └── test_variables.sh
├── functional/
│   ├── mandatory_features.sh
│   ├── modular_features.sh
│   └── edge_cases.sh
└── compare_with_bash.sh
```

**Test Script Template:**
```bash
#!/bin/bash

# tests/compare_with_bash.sh

SHELL="./42sh"
REF_SHELL="bash"
PASSED=0
FAILED=0

test_command() {
    local cmd="$1"
    local desc="$2"
    
    bash_out=$(bash -c "$cmd" 2>&1)
    bash_ret=$?
    
    our_out=$($SHELL -c "$cmd" 2>&1)
    our_ret=$?
    
    if [ "$bash_out" = "$our_out" ] && [ $bash_ret -eq $our_ret ]; then
        echo "✅ PASS: $desc"
        ((PASSED++))
    else
        echo "❌ FAIL: $desc"
        echo "  Command: $cmd"
        echo "  Expected: [$bash_ret] $bash_out"
        echo "  Got:      [$our_ret] $our_out"
        ((FAILED++))
    fi
}

# Run tests
test_command "echo hello" "Simple echo"
test_command "ls | grep test" "Simple pipe"
test_command "echo test > /tmp/test.txt && cat /tmp/test.txt" "Redirect and logical"
# ... many more tests

echo ""
echo "========================================="
echo "Passed: $PASSED"
echo "Failed: $FAILED"
echo "========================================="

[ $FAILED -eq 0 ] && exit 0 || exit 1
```

---

### 8.3 Memory & Stability Testing

**Valgrind Testing:**
```bash
#!/bin/bash
# tests/valgrind_test.sh

valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./42sh -c "echo hello | cat"

if grep -q "ERROR SUMMARY: 0 errors" valgrind-out.txt; then
    echo "✅ No memory errors"
else
    echo "❌ Memory errors detected"
    cat valgrind-out.txt
    exit 1
fi
```

**Stress Testing:**
```bash
# Long pipelines
./42sh -c "cat /etc/passwd | grep root | cut -d: -f1 | tr a-z A-Z | rev | sort"

# Many redirections
./42sh -c "cat < file1 > file2 2> errors.txt"

# Complex nesting
./42sh -c "echo \$(echo \$(echo \$(echo nested)))"

# Large input
./42sh < huge_script.sh

# Many variables
for i in {1..1000}; do
    echo "VAR$i=value$i"
done | ./42sh
```

---

### 8.4 Continuous Testing

**Git Hooks (`.git/hooks/pre-commit`):**
```bash
#!/bin/bash

echo "Running tests before commit..."

make re
if [ $? -ne 0 ]; then
    echo "❌ Compilation failed"
    exit 1
fi

./tests/compare_with_bash.sh
if [ $? -ne 0 ]; then
    echo "❌ Tests failed"
    exit 1
fi

echo "✅ All tests passed"
exit 0
```

---

## 9. Risk Management

### 9.1 Common Pitfalls & Mitigation

| Risk | Impact | Probability | Mitigation |
|------|--------|-------------|------------|
| Feature creep | High | High | Stick to roadmap, phase gates |
| Unstable codebase | Critical | Medium | Test after every feature |
| Team coordination issues | High | Medium | Daily standups, clear interfaces |
| Memory leaks | Critical | High | Valgrind in CI, code reviews |
| Signal handling bugs | High | Medium | Dedicated testing, research |
| Parser complexity explosion | High | Medium | Keep grammar simple, incremental |
| Integration problems | High | Medium | Regular integration, clear APIs |
| Burnout | Medium | Medium | Sustainable pace, breaks |

---

### 9.2 Red Flags & Escalation

**🚩 Stop and Reassess If:**
- Segfaults appearing frequently
- Memory leaks piling up
- Can't explain how your code works
- Tests constantly breaking
- Team members overwhelmed
- Behind schedule by >2 weeks

**Escalation Protocol:**
1. **Team Discussion:** Identify root cause
2. **Simplify:** Cut features if needed
3. **Refactor:** Fix architecture if needed
4. **Seek Help:** Peers, seniors, evaluators
5. **Reset:** Go back to last stable version if necessary

---

## 10. Code Quality Guidelines

### 10.1 Coding Standards

**General Rules:**
```c
// ✅ GOOD
int execute_pipeline(t_ast_node *pipeline, t_shell *shell)
{
    const int cmd_count = count_pipeline_commands(pipeline);
    // ...
}

// ❌ BAD
int ex_pp(t_ast_node *n, t_shell *s)
{
    int cc = cnt_pp_cmd(n);
    // ...
}
```

**Naming Conventions:**
- Functions: `verb_noun()` - `execute_command()`, `parse_token()`
- Structs: `s_noun` - `s_ast_node`, `s_token`
- Enums: `e_category` - `e_token_type`, `e_job_status`
- Constants: `UPPER_CASE` - `MAX_PATH_LEN`, `TOKEN_PIPE`

**Function Length:**
- Maximum ~50 lines (use subfunctions)
- One clear purpose per function
- Clear error handling

**Comments:**
```c
// ✅ GOOD - Explain WHY, not WHAT
// We need to ignore SIGTTOU to prevent background jobs from stopping
// when they try to write to the terminal
signal(SIGTTOU, SIG_IGN);

// ❌ BAD - Obvious comment
// Set signal to ignore
signal(SIGTTOU, SIG_IGN);
```

---

### 10.2 Git Workflow

**Branch Strategy:**
```
main (always stable)
  ├── develop (integration branch)
  │   ├── develop0-setup
  │   ├── develop1-mvp
  │   ├── develop2-mandatory
  │   ├── develop3-modular
  │   ├── develop4-polish
  │   └── develop5-bonus
  │
  └── feature branches (created as needed)
      ├── feature/lexer-quotes
      ├── feature/executor-pipes
      ├── feature/job-control
      └── bugfix/memory-leak-parser
```

**Commit Message Format:**
```
[COMPONENT] Short description (50 chars max)

Longer explanation if needed (wrap at 72 chars).
- Bullet points for multiple changes
- Reference issues: Fixes #123

Examples:
[PARSER] Add support for single quote inhibition
[EXECUTOR] Implement pipeline execution with multiple pipes
[BUILTIN] Add cd with POSIX-compliant options
[TEST] Add integration tests for redirections
[FIX] Fix memory leak in token freeing
[REFACTOR] Simplify AST node creation
```

**Code Review Checklist:**
```
□ Code compiles without warnings
□ Follows naming conventions
□ Functions are reasonably sized
□ No obvious memory leaks
□ Error cases handled
□ Tests added/updated
□ Comments where necessary
□ No debug code (printf, etc.)
```

---

### 10.3 Documentation Requirements

**README.md Structure:**
```markdown
# 42sh - A POSIX-like Shell

## Team
- Nico (Parser)
- Eva (Parser)
- Hugo (Executor)
- Marc (Executor)

## Features
### Mandatory
- [x] Command execution with PATH
- [x] Pipes and redirections
...

### Modular
- [x] Quote handling
- [x] Globbing
...

## Building
```bash
make
```

## Usage
```bash
./42sh
42sh$ echo hello world
```

## Testing
```bash
make test
```

## Architecture
[Brief overview]

## Known Limitations
[If any]
```

**Code Documentation:**
```c
/**
 * execute_pipeline - Execute a pipeline of commands
 * @pipeline: AST node representing the pipeline
 * @shell: Shell state
 *
 * Creates pipes between commands and executes them in parallel.
 * Waits for all commands to complete before returning.
 *
 * Return: Exit status of the last command in the pipeline
 */
int execute_pipeline(t_ast_node *pipeline, t_shell *shell)
{
    // Implementation
}
```

---

## 11. Team Communication & Workflow

### 11.1 Meeting Schedule

**Daily Standup (15 min) - Every morning:**
- What did I do yesterday?
- What will I do today?
- Any blockers?

**Weekly Integration (2-3 hours) - Every Friday:**
- Merge parser + executor work
- Integration testing
- Plan next week

**Bi-weekly Retrospective (1 hour) - Every other Friday:**
- What went well?
- What can improve?
- Adjust process

---

### 11.2 Communication Channels

**Synchronous:**
- In-person coding sessions
- Video calls for remote work
- Pair programming sessions

**Asynchronous:**
- Slack/Discord for quick questions
- GitHub issues for bugs/features
- GitHub PR comments for code review

**Documentation:**
- This document (living document - update as needed!)
- Code comments
- Git commit messages
- Wiki for complex topics

---

## 12. Success Criteria

### 12.1 Definition of Done (Per Phase)

**Phase Done When:**
- ✅ All features in goal completed
- ✅ All tests passing
- ✅ No memory leaks (valgrind clean)
- ✅ No segfaults on valid input
- ✅ Code reviewed by team
- ✅ Merged to main develop branch
- ✅ Documentation updated

**Project Done When:**
- ✅ All mandatory features work perfectly
- ✅ 6+ modular features work perfectly
- ✅ Comprehensive test suite passes
- ✅ Zero memory leaks
- ✅ Zero segfaults (even on invalid input)
- ✅ Clean code (no ft_parse1, etc.)
- ✅ Good git history
- ✅ README complete
- ✅ Ready for defense

---

### 12.2 Defense Preparation

**What Evaluators Will Look For:**
1. **Stability** - Try to break your shell
2. **Feature completeness** - Check all mandatory + modular
3. **Code quality** - Readable, maintainable
4. **Understanding** - Can you explain your code?
5. **Edge cases** - Unusual inputs, error handling

**Demo Script (Prepare in advance):**
```bash
# 1. Basic commands
echo "Hello from 42sh!"
ls -la
pwd

# 2. Pipes
cat file.txt | grep test | wc -l

# 3. Redirections
echo "test" > output.txt
cat < output.txt
cat << EOF
heredoc
test
EOF

# 4. Logical operators
echo "first" && echo "second"
false || echo "fallback"

# 5. Variables
VAR=hello
echo $VAR
export VAR
unset VAR

# 6. Job control
sleep 100 &
jobs
fg %1
^Z
bg %1

# 7. Modular features
echo *.c
echo $(ls)
cd ~
# etc.

# 8. Error handling
cat /nonexistent
ls /etc | grep test | invalid_command
```

---

## 13. Resources & References

### 13.1 Essential Reading

**POSIX Standards:**
- https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html

**Shell Implementation Guides:**
- "Unix Programming FAQ" - https://www.faqs.org/faqs/unix-faq/
- "Writing a Unix Shell" - Stephen Brennan
- "Build Your Own Shell" - tutorials

**Signal Handling:**
- `man 7 signal`
- `man 2 sigaction`
- Job Control: https://www.gnu.org/software/libc/manual/html_node/Job-Control.html

**Process Management:**
- `man 2 fork`
- `man 2 execve`
- `man 2 wait`
- `man 2 pipe`

---

### 13.2 Useful Commands for Research

**Compare shell behaviors:**
```bash
# Test in different shells
bash -c "command" > bash.out 2>&1
sh -c "command" > sh.out 2>&1
zsh -c "command" > zsh.out 2>&1

# Compare
diff bash.out sh.out
```

**Debug existing shells:**
```bash
# Trace system calls
strace bash -c "ls | grep test" 2>&1 | less

# See what bash does
bash -x script.sh
```

**Test signal handling:**
```bash
# See how bash handles signals
bash -c "trap 'echo SIGINT caught' INT; sleep 100"
# Press Ctrl+C
```

---

## 14. Appendix

### 14.1 Project Structure

```
42sh/
├── Makefile
├── README.md
├── .gitignore
│
├── includes/
│   ├── shell.h          # Main header
│   ├── lexer.h          # Lexer/tokenizer
│   ├── parser.h         # Parser/AST
│   ├── executor.h       # Executor
│   ├── builtins.h       # Built-in commands
│   └── utils.h          # Utilities
│
├── srcs/
│   ├── main.c           # Entry point, REPL
│   │
│   ├── lexer/           # PARSER TEAM
│   │   ├── tokenize.c
│   │   ├── token_utils.c
│   │   └── quote_handler.c
│   │
│   ├── parser/          # PARSER TEAM
│   │   ├── parse.c
│   │   ├── parse_command.c
│   │   ├── parse_pipeline.c
│   │   ├── parse_operators.c
│   │   └── ast_utils.c
│   │
│   ├── expansion/       # PARSER TEAM (collaboration with executor)
│   │   ├── expand_vars.c
│   │   ├── expand_globs.c
│   │   ├── expand_tilde.c
│   │   └── expand_command_sub.c
│   │
│   ├── executor/        # EXECUTOR TEAM
│   │   ├── execute.c
│   │   ├── execute_command.c
│   │   ├── execute_pipeline.c
│   │   ├── execute_operators.c
│   │   └── redirections.c
│   │
│   ├── builtins/        # EXECUTOR TEAM
│   │   ├── builtin_cd.c
│   │   ├── builtin_echo.c
│   │   ├── builtin_exit.c
│   │   ├── builtin_type.c
│   │   ├── builtin_export.c
│   │   ├── builtin_set.c
│   │   ├── builtin_unset.c
│   │   ├── builtin_jobs.c
│   │   ├── builtin_fg.c
│   │   └── builtin_bg.c
│   │
│   ├── job_control/     # EXECUTOR TEAM
│   │   ├── jobs.c
│   │   ├── process_groups.c
│   │   └── terminal_control.c
│   │
│   ├── signals/         # ALL TEAM
│   │   └── signal_handlers.c
│   │
│   ├── variables/       # EXECUTOR TEAM (collaboration with parser)
│   │   ├── var_table.c
│   │   ├── environment.c
│   │   └── var_utils.c
│   │
│   └── utils/           # ALL TEAM
│       ├── error.c
│       ├── memory.c
│       ├── string_utils.c
│       └── path_utils.c
│
├── libft/               # Your libft
│   ├── Makefile
│   └── ...
│
└── tests/
    ├── unit/
    ├── integration/
    ├── functional/
    └── compare_with_bash.sh
```

---

### 14.2 Initial Makefile

```makefile
# Makefile for 42sh

NAME = 42sh

# Directories
SRCS_DIR = srcs
OBJS_DIR = objs
INCS_DIR = includes
LIBFT_DIR = libft

# Source files (add as you create them)
SRCS = main.c \
       lexer/tokenize.c \
       parser/parse.c \
       executor/execute.c \
       builtins/builtin_cd.c \
       builtins/builtin_echo.c \
       builtins/builtin_exit.c \
       # ... add more as you go

OBJS = $(SRCS:%.c=$(OBJS_DIR)/%.o)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I$(INCS_DIR) -I$(LIBFT_DIR)/includes
LDFLAGS = -L$(LIBFT_DIR) -lft -lreadline

# Libft
LIBFT = $(LIBFT_DIR)/libft.a

# Colors for output
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m

# Rules
all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	@echo "$(GREEN)Linking $(NAME)...$(RESET)"
	@$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(GREEN)$(NAME) created successfully!$(RESET)"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(LIBFT):
	@echo "$(GREEN)Building libft...$(RESET)"
	@make -C $(LIBFT_DIR)

clean:
	@echo "$(RED)Cleaning object files...$(RESET)"
	@rm -rf $(OBJS_DIR)
	@make -C $(LIBFT_DIR) clean

fclean: clean
	@echo "$(RED)Removing $(NAME)...$(RESET)"
	@rm -f $(NAME)
	@make -C $(LIBFT_DIR) fclean

re: fclean all

test: all
	@echo "$(GREEN)Running tests...$(RESET)"
	@bash tests/compare_with_bash.sh

.PHONY: all clean fclean re test
```

---

### 14.3 Quick Reference: Shell Grammar (Simplified)

```
command_line    : pipeline_list
pipeline_list   : pipeline_list separator pipeline | pipeline
separator       : ';' | '&' | '&&' | '||'
pipeline        : command | pipeline '|' command
command         : simple_command | compound_command
simple_command  : word_list redirection_list
word_list       : word | word_list word
redirection_list: redirection | redirection_list redirection
redirection     : '>' word | '<' word | '>>' word | '<<' word | '>&' word | '<&' word
compound_command: '(' command_line ')' | '{' command_list '}'
```

---

### 14.4 Exit Status Reference

| Status | Meaning |
|--------|---------|
| 0 | Success |
| 1 | General error |
| 2 | Misuse of shell built-in |
| 126 | Command cannot execute (permission) |
| 127 | Command not found |
| 128+N | Signal N terminated the process |
| 130 | Ctrl+C (SIGINT = 2, 128+2=130) |

---

## 15. Final Thoughts & Motivation

### You've Got This! 🚀

This is a **massive** project, but you have:
- ✅ **Experience** from minishell
- ✅ **A solid team** with complementary skills
- ✅ **A clear roadmap** (this document!)
- ✅ **The right mindset** (stability over features)

### Remember:
1. **Don't panic** - Take it goal by goal (develop0 → develop1 → ...)
2. **Communicate** - Talk to your team daily
3. **Test early, test often** - Catch bugs early
4. **Ask for help** - Peers, seniors, the internet
5. **Take breaks** - Burnout helps nobody
6. **Have fun** - You're building something cool!

### The Journey:
```
develop0: 😰 "This is impossible!"
develop1: 😅 "Okay, we can do this..."
develop2: 😊 "It's actually working!"
develop3: 😎 "We're adding cool features!"
develop4: 🎯 "Polish time!"
Defense:  🎉 "ONE FOR THE AGES!"
```

---

**Good luck, team! You're going to build something amazing. 💪**

**Remember: A humble, stable shell beats a fancy, broken shell every time.**

---

## Document Version History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | Nov 15, 2025 | GitHub Copilot | Initial comprehensive document |
| 1.1 | Nov 15, 2025 | GitHub Copilot | Changed to goal-based structure (develop0-5) |

---

**END OF DOCUMENT**

*This is a living document. Update it as your project evolves!*
