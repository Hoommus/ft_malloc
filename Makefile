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
                    -fPIC
#                    -lpthread \
                    #-Og -fsanitize="address"

SRC_DIR = ./src/
OBJ_DIR = ./obj/

LIB_DIR = ./libft
LIB_NAME = libft.a

MALLOC_SRC = free.c \
	         realloc.c \
	         malloc.c

HEADERS = -I includes/ -I $(LIB_DIR) -I $(PRINTF_DIR)

OBJ = $(addprefix $(OBJ_DIR), $(MALLOC_SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ) $(LIB_NAME)
	clang $(FLAGS) --shared -o $(NAME) $(OBJ) $(HEADERS) $(LIB_DIR)/$(LIB_NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c | $(OBJ_DIR)
	clang $(FLAGS) $(HEADERS) -o $@ -c $< ;

$(LIB_NAME):
	make -C $(LIB_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

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
