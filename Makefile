##
## EPITECH PROJECT, 2024
## Makefile
## File description:
## Makefile
##

CC	=	g++

SRC_DIR = ./

SRC	=	$(wildcard $(SRC_DIR)*.cpp)

OBJ	=	$(SRC:.c=.o)

NAME	=	ecs

CFLAGS	=	-Wall -Wextra -W -g3 -g -lsfml-graphics -lsfml-window -lsfml-system

all: $(NAME)

$(NAME):	$(OBJ)
	$(CC) $(OBJ) -o $(NAME) $(CFLAGS)

clean:
	rm -f *.o
	rm -f *~

fclean: clean
	rm -f $(NAME)

re: fclean all
