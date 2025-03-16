NAME	:= ft_traceroute

SRC		:=	src/main.c \
			src/utils.c \
			src/traceroute.c

BUILD_DIR	:= build

OBJ		:= $(addprefix $(BUILD_DIR)/,$(SRC:.c=.o))

CC		:= gcc
CFLAGS	:= #-Wall -Wextra -Werror

LIB_PATH	:= my_42_lib/lib

all: create_dir $(NAME)

create_dir:
	@mkdir -p $(BUILD_DIR)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -L${LIB_PATH} -lopt -lft -llogger

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean

re: fclean all

.PHONY: all clean fclean re create_dir