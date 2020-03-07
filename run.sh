#!/bin/sh

export DYLD_INSERT_LIBRARIES="/Users/vtarasiu/CLionProjects/ft_malloc/libft_malloc.so"
export DYLD_FORCE_FLAT_NAMESPACE=1
$@
