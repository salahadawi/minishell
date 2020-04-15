# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sadawi <sadawi@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/04/10 19:27:20 by sadawi            #+#    #+#              #
#    Updated: 2020/04/15 16:16:32 by sadawi           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell
CFILES = *.c
#SRCS = $(addprefix srcs/, $(CFILES))
SRCS = $(CFILES)
OBJS = $(addprefix objs/, $(notdir $(SRCS:.c=.o)))
INCLUDES = -I includes -I libft/includes
FLAGS = -Wall -Wextra -Werror
RUN_LIB = make -C libft/ fclean && make -C libft/

all: $(NAME)

$(NAME):
	@$(RUN_LIB)
	@rm -rf objs
	@echo Compiling $(NAME)...
	@gcc $(FLAGS) $(INCLUDES) -c $(SRCS)
	@mkdir objs
	@mv $(notdir $(SRCS:.c=.o)) objs
	@gcc $(FLAGS) $(INCLUDES) -o $(NAME) $(OBJS) libft/libft.a
	@echo $(NAME) compiled succesfully!

lib:
	@$(RUN_LIB)

noflags:
	@rm -rf objs
	@echo Compiling $(NAME) without flags...
	@gcc $(INCLUDES) -c $(SRCS)
	@mkdir objs
	@mv $(notdir $(SRCS:.c=.o)) objs
	@gcc $(INCLUDES) -o $(NAME) $(OBJS) libft/libft.a
	@echo $(NAME) compiled without flags succesfully!

clean:
	@/bin/rm -f $(OBJS)
	@rm -rf objs
	@make -C libft/ clean
	@echo Clean successful!

fclean: clean
	@/bin/rm -f $(NAME)
	@make -C libft/ fclean
	@echo Clean successful!

re: fclean all
