# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vtarasiu <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/07/12 14:31:34 by vtarasiu          #+#    #+#              #
#    Updated: 2020/03/07 15:36:08 by vtarasiu         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

CC = clang

NAME = libft_malloc_$(HOSTTYPE).so

FLAGS = -g \
        -Wall \
        -Wextra \
        -Werror \
        -Wno-unknown-pragmas
#                    -lpthread

SRC_DIR = ./src/
OBJ_DIR = ./obj/

LIB_DIR = ./libft
LIB_NAME = libft.a

MALLOC_SRC = realloc.c \
             auxiliary.c \
             region_management.c \
             zones_management.c \
             allocator.c \
             free.c \
             malloc.c \
             allocator_reverse.c \
             init.c

HEADERS = -I includes/ -I $(LIB_DIR)

OBJ = $(addprefix $(OBJ_DIR), $(MALLOC_SRC:.c=.o))

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJ) $(LIB_NAME)
	$(CC) -shared -flat_namespace -o $(NAME) $(OBJ) $(LIB_DIR)/$(LIB_NAME) $(HEADERS)
	@ if ! [ -f libft_malloc.so ] ; then \
	    /bin/ln -s $(NAME) libft_malloc.so ; \
	fi

$(OBJ_DIR)%.o: $(SRC_DIR)%.c includes/ft_malloc_private.h | $(OBJ_DIR)
	$(CC) $(FLAGS) -fPIC -fvisibility=hidden $(HEADERS) -o $@ -c $< ;

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
