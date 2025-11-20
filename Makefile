# -=-=-=-=-    NAME -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

NAME		= 	42sh

# -=-=-=-=-    FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRC			=	/main/main.c					\
				/main/init.c					\
				/main/prompt.c					\
				/main/loop.c					\
				/env/env_manager.c				\
				/executor/executor_manager.c	\
				/executor/cmd_cache.c			\
				/builtins/builtins_manager.c	\

SRCDIR		=	srcs
SRCS		=	$(addprefix $(SRCDIR)/, $(SRC))

OBJDIR		=	.obj
OBJS		=	$(addprefix $(OBJDIR)/, $(SRC:.c=.o))

DEPDIR		=	.dep
DEPS		=	$(addprefix $(DEPDIR)/, $(SRC:.c=.d))

INC			=	./incs/
HEADERS		=	./incs/42sh.h 			\
				./incs/main.h	 		\
				./incs/env.h			\
				./incs/executor.h		\
				./incs/builtins.h
INCLUDES	=	-I./incs/
LIBFT		=	./libft/libft.a

MAKE		=	Makefile
			
# -=-=-=-=-    FLAGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -pedantic -g -fsanitize=address
DEPFLAGS	=	-MMD -MP

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: directories libs $(NAME)

directories:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(DEPDIR)

libs:
	@make -C ./libft/

-include $(DEPS)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -lreadline -o $(NAME)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c Makefile
	mkdir -p $(@D)
	mkdir -p $(DEPDIR)/$(*D)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -I./libft/incs/ -c $< -o $@ -MF $(DEPDIR)/$*.d
	
clean:
	@/bin/rm -fr $(OBJDIR) $(DEPDIR)
	@make -C ./libft clean

fclean: clean
	@/bin/rm -f $(NAME)
	@make -C ./libft fclean

re: fclean all

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind.supp ./$(NAME)

.PHONY:  all clean fclean re libs valgrind directories
