# Header

# ***************************************************** #
# *                 Configuration                     * #
# ***************************************************** #

MAKEFLAGS += --no-print-directory

# ***************************************************** #
# *                    Variables                      * #
# ***************************************************** #

NAME		:= interface.a

CC			:= cc
CFLAGS		?= -Wall -Wextra -Werror -MMD
DEBUGFLAGS	?=
INCLUDES	:=  -I../config -I../standards -I../

INTERNAL	?= _internal_

DIR_BUILD	:= .build
DIR_IO		:= io
DIR_JSON	:= json
DIR_LOG		:= log

SRC_IO		:= 
_SRC_IO	:= #$(addprefix $(DIR_IO)/, $(SRC_IO))

SRC_JSON	:=	json.c $(INTERNAL)/_access.c $(INTERNAL)/_memory.c \
				$(INTERNAL)/_parser.c $(INTERNAL)/_set.c $(INTERNAL)/_tools.c \
				$(INTERNAL)/_write.c
_SRC_JSON	:= $(addprefix $(DIR_JSON)/, $(SRC_JSON))

SRC_LOG		:= log.c $(INTERNAL)/_log.c
_SRC_LOG	:= $(addprefix $(DIR_LOG)/, $(SRC_LOG))

_SRC_ALL	:= $(_SRC_JSON) $(_SRC_LOG) $(_SRC_IO)

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