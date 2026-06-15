NAME	= ft_ping
CC		= cc
CFLAGS	= -Wall -Werror -Wextra -Iheader

SRC_DIR = src
OBJ_DIR	= obj
HDR_DIR = header

SRCS	= main.c init.c packet.c utils.c
OBJS	= $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
HEADERS = $(HDR_DIR)/ping.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re