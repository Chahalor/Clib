NAME := modulesmng

CC := cc
CMAKE := cmake

CLIB_ROOT := ..
CLIB_BUILD := $(CLIB_ROOT)/build
CLIB_LIB := $(CLIB_BUILD)/libclib.a

SRC_DIR := src
OBJ_DIR := build/obj

INSTALL_DIR	:= ~/.local/bin

SRCS := $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

CPPFLAGS := \
	-I$(SRC_DIR) \
	-I$(CLIB_ROOT) \
	-I$(CLIB_ROOT)/_internal_ \
	-I$(CLIB_ROOT)/standards \
	-I$(CLIB_ROOT)/files \
	-I$(CLIB_ROOT)/interface \
	-I$(CLIB_ROOT)/memory \
	-I$(CLIB_ROOT)/network \
	-I$(CLIB_ROOT)/random \
	-I$(CLIB_ROOT)/times

CFLAGS := -Wall -Wextra -Werror -g3 -O0 -MMD -MP
LDFLAGS :=
LDLIBS := $(CLIB_LIB)

.PHONY: all clib clean fclean re FORCE

all: $(NAME)

$(NAME): $(OBJS) $(CLIB_LIB)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $@

clib: $(CLIB_LIB)

$(CLIB_LIB): FORCE
	$(CMAKE) -S $(CLIB_ROOT) -B $(CLIB_BUILD)
	$(CMAKE) --build $(CLIB_BUILD)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r build

fclean: clean
	$(RM) $(NAME)

re: fclean all

install: all
	cp ./$(NAME) $(INSTALL_DIR)/

-include $(DEPS)
