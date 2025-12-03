# -=-=-=-=-    NAME -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

NAME			= 	42sh

# -=-=-=-=-    FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRC				=	/main/main.c					\
					/main/init.c					\
					/main/prompt.c					\
					/main/loop.c					\
					/token/token.c					\
					/token/splitter.c				\
					/token/continuation.c			\
					/token/quotes.c					\
					/env/env_manager.c				\
					/executor/executor_manager.c	\
					/executor/cmd_cache.c			\
					/builtins/builtins_manager.c	\

SRCDIR			=	srcs
SRCS			=	$(addprefix $(SRCDIR)/, $(SRC))

OBJDIR			=	.obj
OBJS			=	$(addprefix $(OBJDIR)/, $(SRC:.c=.o))

DEPDIR			=	.dep
DEPS			=	$(addprefix $(DEPDIR)/, $(SRC:.c=.d))

TEST_DIR		=	tests
TEST_SRCS		=	$(wildcard $(TEST_DIR)/*.c)
TEST_OBJS		=	$(TEST_SRCS:.c=.o)
TEST_NAME		=	test_runner
TEST_CFLAGS		=	$(CFLAGS) -DTESTING

# Check for Criterion installation (local first, then system)
LOCAL_CRITERION_INCLUDE = $(HOME)/.local/include
LOCAL_CRITERION_LIB = $(HOME)/.local/lib/x86_64-linux-gnu

# First check for local installation
ifneq ($(wildcard $(LOCAL_CRITERION_INCLUDE)/criterion/criterion.h),)
	TEST_INCLUDES = $(INCLUDES) -I$(TEST_DIR) -I$(LOCAL_CRITERION_INCLUDE)
	TEST_LDFLAGS = -L$(LOCAL_CRITERION_LIB) -lcriterion
	TEST_LD_PATH = LD_LIBRARY_PATH=$(LOCAL_CRITERION_LIB):$$LD_LIBRARY_PATH
	CRITERION_AVAILABLE = 1
	CRITERION_SOURCE = local
# Then check for system installation via pkg-config
else ifeq ($(shell pkg-config --exists criterion 2>/dev/null && echo yes),yes)
	SYSTEM_CRITERION_INCLUDE = $(shell pkg-config --cflags criterion)
	SYSTEM_CRITERION_LIB = $(shell pkg-config --libs criterion)
	TEST_INCLUDES = $(INCLUDES) -I$(TEST_DIR) $(SYSTEM_CRITERION_INCLUDE)
	TEST_LDFLAGS = $(SYSTEM_CRITERION_LIB)
	TEST_LD_PATH = 
	CRITERION_AVAILABLE = 1
	CRITERION_SOURCE = system
# Finally check for basic system installation without pkg-config
else ifneq ($(wildcard /usr/include/criterion/criterion.h),)
	TEST_INCLUDES = $(INCLUDES) -I$(TEST_DIR) -I/usr/include
	TEST_LDFLAGS = -lcriterion
	TEST_LD_PATH = 
	CRITERION_AVAILABLE = 1
	CRITERION_SOURCE = system-basic
# No Criterion found
else
	TEST_INCLUDES = $(INCLUDES) -I$(TEST_DIR)
	TEST_LDFLAGS = 
	TEST_LD_PATH = 
	CRITERION_AVAILABLE = 0
	CRITERION_SOURCE = none
endif

INC				=	./incs/
HEADERS			=	./incs/42sh.h 			\
					./incs/main.h	 		\
					./incs/env.h			\
					./incs/executor.h		\
					./incs/builtins.h
INCLUDES		=	-I./incs/
LIBFT			=	./libft/libft.a

MAKE			=	Makefile
			
# -=-=-=-=-    FLAGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

CC				=	cc
CFLAGS			?=	-Wall -Wextra -Werror -pedantic -g# -fsanitize=address
DEPFLAGS		=	-MMD -MP

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: directories libs $(NAME)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(DEPDIR)

libs:
	@make -C ./libft/

test: all check-criterion $(TEST_NAME)
	$(TEST_LD_PATH) ./$(TEST_NAME) --verbose

test-setup:
	@echo "🔧 Setting up Criterion testing framework locally..."
	@echo "This will install Criterion in ~/.local (no sudo required)"
	@echo ""
	@read -p "Continue? [y/N] " confirm && [ "$$confirm" = "y" ] || exit 1
	@echo "Installing build tools..."
	pip3 install --user meson ninja
	@echo "Downloading and building Criterion..."
	mkdir -p deps && cd deps && \
	git clone --recursive https://github.com/Snaipe/Criterion.git && \
	cd Criterion && \
	export PATH=$$HOME/.local/bin:$$PATH && \
	meson setup --prefix=$$HOME/.local builddir && \
	cd builddir && \
	meson compile && \
	meson install
	@echo "✅ Criterion installed! You can now run 'make test'"

check-criterion:
ifeq ($(CRITERION_AVAILABLE),0)
	@echo "❌ Criterion testing framework not found!"
	@echo ""
	@echo "📋 Installation options:"
	@echo "  • Ubuntu/Debian: sudo apt-get install libcriterion-dev"
	@echo "  • macOS: brew install criterion"
	@echo "  • Local install: run 'make test-setup' for automatic local installation"
	@echo ""
	@false
endif
	@echo "✅ Criterion detected ($(CRITERION_SOURCE)) - running tests..."

-include $(DEPS)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -lreadline -o $(NAME)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c Makefile
	mkdir -p $(@D)
	mkdir -p $(DEPDIR)/$(*D)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -I./libft/incs/ -c $< -o $@ -MF $(DEPDIR)/$*.d

$(TEST_NAME): $(TEST_OBJS) $(filter-out $(OBJDIR)//main/main.o, $(OBJS)) $(LIBFT)
	$(CC) $(TEST_CFLAGS) $^ $(TEST_LDFLAGS) -lreadline -o $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(TEST_CFLAGS) $(TEST_INCLUDES) -I./libft/incs/ -c $< -o $@
	
test_clean:
	rm -f $(TEST_OBJS) $(TEST_NAME)

clean: test_clean
	@/bin/rm -fr $(OBJDIR) $(DEPDIR)
	@make -C ./libft clean

fclean: clean
	@/bin/rm -f $(NAME)
	@make -C ./libft fclean

re: fclean all

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind.supp ./$(NAME)

test-debug:
	@echo "🔍 Criterion Detection Debug Information:"
	@echo "  Local header check: $(wildcard $(LOCAL_CRITERION_INCLUDE)/criterion/criterion.h)"
	@echo "  System pkg-config: $(shell pkg-config --exists criterion 2>/dev/null && echo yes || echo no)"
	@echo "  System header check: $(wildcard /usr/include/criterion/criterion.h)"
	@echo "  Criterion available: $(CRITERION_AVAILABLE)"
	@echo "  Criterion source: $(CRITERION_SOURCE)"
	@echo "  Test includes: $(TEST_INCLUDES)"
	@echo "  Test ldflags: $(TEST_LDFLAGS)"
	@echo "  Test LD_PATH: $(TEST_LD_PATH)"

.PHONY:  all clean fclean re libs valgrind directories test test-setup check-criterion test_clean test-debug
