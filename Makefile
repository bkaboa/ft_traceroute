NAME	:= ft_traceroute

SRC		:=	src/main.c \
			src/utils.c

BUILD_DIR	:= build

OBJ		:= $(addprefix $(BUILD_DIR)/,$(SRC:.c=.o))

CC		:= gcc
CFLAGS	:= #-Wall -Wextra -Werror

LIB_PATH	:= my_42_lib/lib

LOGGER_LIB	:= ${LIB_PATH}/liblogger.a
OPT_LIB		:= ${LIB_PATH}/libopt.a
FT_LIB		:= ${LIB_PATH}/libft.a

HEADERS	:= include/ft_traceroute.h

LIBS := -L${LIB_PATH} -lopt -lft -llogger

all: create_dir libs $(NAME)

create_dir:
	@mkdir -p $(BUILD_DIR)

$(NAME): $(OBJ) $(LOGGER_LIB) $(OPT_LIB) $(FT_LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^ -I include -I my_42_lib

libs:
	@make -C my_42_lib/build

clean:
	@rm -rf $(BUILD_DIR)

fclean: clean

re: fclean all

.PHONY: all clean fclean re create_dir libs