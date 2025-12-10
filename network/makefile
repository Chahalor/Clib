# Header

# ***************************************************** #
# *                 Configuration                     * #
# ***************************************************** #

MAKEFLAGS += --no-print-directory

# ***************************************************** #
# *                    Variables                      * #
# ***************************************************** #

NAME		:= network.a

CC			:= cc
CFLAGS		?= -Wall -Wextra -Werror -MMD
DEBUGFLAGS	?=
INCLUDES	:= -I../config -I../standards -I../

INTERNAL	?= _internal_

DIR_BUILD	:= .build
DIR_SERVER	:= $(INTERNAL)/server
DIR_IO		:= $(INTERNAL)/io
DIR_CLIENT	:= $(INTERNAL)/client

SRC			:= network.c $(DIR_SERVER)/_server.c $(DIR_IO)/_io.c $(DIR_CLIENT)/_client.c

_SRC_ALL	:= $(SRC)

_OBJ		:= $(addprefix $(DIR_BUILD)/, $(_SRC_ALL:.c=.o))
_DEPS		:= $(_OBJ:.o=.d)

-include $(_DEPS)

# ***************************************************** #
# *                    Rules                          * #
# ***************************************************** #

.PHONY: all

all: $(NAME)

# ***************************************************** #
# *                  Compiling                        * #
# ***************************************************** #

$(NAME): $(_OBJ)
# 	$(CC) $(CFLAGS) $(DEBUGFLAGS) $^ <libs> -o $(NAME)
	ar rcs $(NAME) $^

$(DIR_BUILD):
	mkdir -p $@ $(DIR_BUILD)/$(INTERNAL)
	mkdir -p $(dir $(_OBJ))

$(DIR_BUILD)/%.o: %.c | $(DIR_BUILD)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@

# ***************************************************** #
# *                    Clean Rules                    * #
# ***************************************************** #

.PHONY: clean fclean re

clean:
	rm $(_OBJ)

fclean:
	rm -rf $(DIR_BUILD) $(NAME)

re: fclean all

# ***************************************************** #
# *                    Debug Rules                    * #
# ***************************************************** #

# ...

# ***************************************************** #
# *                      Utils                        * #
# ***************************************************** #

# ...