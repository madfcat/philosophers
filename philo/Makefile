CC = cc
CFLAGS = -Wall -Wextra -Werror
# CFLAGS = -Wall -Wextra -Werror -fsanitize=thread

NAME = philo
HEADER = philo.h

SRCS = philo.c \
	philo_utils.c \
	ft_atoi.c

OBJ_DIR = obj/
OBJS = $(SRCS:.c=.o)
OBJECTS = $(addprefix $(OBJ_DIR), $(OBJS))

all: $(NAME)

$(NAME): $(SRCS) $(HEADER)
	$(CC) $(CFLAGS) -c $(SRCS) -I.
	mkdir -p $(OBJ_DIR)
	mv $(OBJS) $(OBJ_DIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re