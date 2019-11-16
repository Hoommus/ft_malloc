# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <vtarasiu@student.unit.ua>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/07/12 14:31:34 by vtarasiu          #+#    #+#              #
#    Updated: 2019/07/12 14:40:53 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so

FLAGS = -g -Wall \
                    -Wextra \
                    -Werror \
                    -Wno-unknown-pragmas \
                    --shared
                    -fPIC
#                    -lpthread \
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
