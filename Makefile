# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nponchon <nponchon@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/17 12:08:55 by nponchon          #+#    #+#              #
#    Updated: 2025/11/17 15:22:22 by nponchon         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# -=-=-=-=-    NAME -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

NAME		= 	42sh

# -=-=-=-=-    FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

SRC			=	main.c

SRCDIR		=	srcs
SRCS		=	$(addprefix $(SRCDIR)/, $(SRC))

OBJDIR		=	.obj
OBJS		=	$(addprefix $(OBJDIR)/, $(SRC:.c=.o))

INC			=	./incs/
HEADERS		=	./incs/42sh.h ./libft/libft.h
INCLUDES	=	-I
LIBFT		=	./libft/libft.a

MAKE		=	Makefile
			
# -=-=-=-=-    FLAGS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

CC			=	cc
CFLAGS		=	-Wall -Wextra -Werror -pedantic -g

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: libs $(NAME)

libs:
	@make -C ./libft/

$(NAME): $(OBJS) $(HEADERS) $(SRCS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) Makefile
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES)./libft/ -c $< -o $@
	
clean:
	@/bin/rm -fr $(OBJDIR)
	@make -C ./libft clean

fclean: clean
	@/bin/rm -f $(NAME)
	@make -C ./libft fclean

re: fclean all

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind.supp ./$(NAME)

.PHONY:  all clean fclean re libs valgrind
