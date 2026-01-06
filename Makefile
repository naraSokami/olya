NAME := test

C_CC := gcc
CPP_CC := g++
CFLAGS := -Wall -Wextra -Werror

OBJS_DIR := objs

C_SRC := ft_split.c
CPP_SRC := test.cpp
C_OBJ := $(C_SRC:.c=.o)
CPP_OBJ := $(CPP_SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(C_OBJ) $(CPP_OBJ)
	$(CPP_CC) $(CPP_OBJ) $(C_OBJ) $(CFLAGS) -o $(NAME)

$(C_OBJ): %.o: %.c
	$(C_CC) $(FLAGS) -c $< -o $@

$(CPP_OBJ): %.o: %.cpp
	$(CPP_CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(C_OBJ) $(CPP_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
