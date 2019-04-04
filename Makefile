# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hoommus <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/03/24 10:11:17 by vtarasiu          #+#    #+#              #
#    Updated: 2019/04/04 14:49:20 by hoommus          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so

FLAGS = -g -std=c11 -Wall \
                    -Wextra \
                    -Werror \
                    -Wno-unknown-pragmas \
                    --shared \
                    #-Og -fsanitize="address"

HEADER = -I include/ -I libft/
SRC_DIR = ./src/
OBJ_DIR = ./obj/

LIB_DIR = ./libft
LIB_NAME = libft.a

MALLOC_SRC = main.c

OBJ = $(addprefix $(OBJ_DIR), $(MALLOC_SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ)
	clang $(FLAGS) -o $(NAME) $(OBJ) $(HEADER) $(LIB_NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	clang $(FLAGS) $(HEADER) -o $@ -c $< ;

clean:
	make -C libft clean
	@echo "rm -rf $(OBJ_DIR)*.o"
	@/bin/rm -rf $(OBJ)
	/bin/rm -rf $(OBJ_DIR)

fclean: clean
	make -C $(LIB_DIR) fclean
	/bin/rm -f $(NAME)
	/bin/rm -f $(LIB_NAME)

re: fclean $(NAME)

love:
	@echo "Not all."

.PHONY: clean all fclean re love norme
