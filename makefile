# Header

# ***************************************************** #
# *                 Configuration                     * #
# ***************************************************** #

# MAKEFLAGS += --no-print-directory

# ***************************************************** #
# *                    Variables                      * #
# ***************************************************** #

NAME		:= lib.a

CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror -MMD #more flags
DEBUGFLAGS	:=

DIR_BUILD	= .build
DIR_STAND	= ../standards
DIR_CONFIG	= ../config
DIR_INTERN	= _internal_

DIR_MEMORY		:= memory
DIR_INTERFACE	:= interface
DIR_LOGS		:= $(DIR_INTERFACE)/log
DIR_NETWORK		:= network

INCLUDES	:= -I$(DIR_CONFIG) -I$(DIR_STAND) -I$(DIR_INTERN) -I$(DIR_NETWORK)

_SRC_ALL	:=

_OBJ		:= $(addprefix $(DIR_BUILD)/, $(_SRC_ALL:.c=.o))
_DEPS		:= $(_OBJ:.o=.d)

-include $(_DEPS)

# ***************************************************** #
# *                    Rules                          * #
# ***************************************************** #

.PHONY: all

all: header $(NAME)

# ***************************************************** #
# *                  Compiling                        * #
# ***************************************************** #

.PHONY: interface memory network

$(NAME): interface memory network
	mkdir -p $(DIR_BUILD)
	ld -r -o .build/fusion.o \
		--whole-archive \
			memory/memory.a \
			interface/interface.a \
			$(DIR_NETWORK)/network.a \
		--no-whole-archive
	ar rcs $(NAME) $(DIR_BUILD)/fusion.o

# 	mkdir -p $(DIR_BUILD)/fusion
# 	cd $(DIR_BUILD)/fusion && ar -x ../../$(DIR_MEMORY)/memory.a  && ar -x ../../$(DIR_INTERFACE)/interface.a
# 	ar rcs $(NAME) $(DIR_BUILD)/fusion/*.o



interface:
	$(MAKE) -C $(DIR_INTERFACE) CFLAGS="$(CFLAGS)" DEBUGFLAGS="$(DEBUGFLAGS)"

memory:
	$(MAKE) -C $(DIR_MEMORY) CFLAGS="$(CFLAGS)" DEBUGFLAGS="$(DEBUGFLAGS)"

network:
	$(MAKE) -C $(DIR_NETWORK) CFLAGS="$(CFLAGS)" DEBUGFLAGS="$(DEBUGFLAGS)"

$(DIR_BUILD):
	mkdir -p $@ $(DIR_BUILD)/_internal_
	mkdir -p $(dir $(_OBJ))

$(DIR_BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@

# ***************************************************** #
# *                    Clean Rules                    * #
# ***************************************************** #

.PHONY: clean fclean re

clean:
	rm $(_OBJ)
	make -C $(DIR_INTERFACE) clean
	make -C $(DIR_MEMORY) clean
	make -C $(DIR_NETWORK) clean

fclean:
	rm -rf $(DIR_BUILD) $(NAME)
	make -C $(DIR_INTERFACE) fclean
	make -C $(DIR_MEMORY) fclean
	make -C $(DIR_NETWORK) fclean

re: fclean all

# ***************************************************** #
# *                    Debug Rules                    * #
# ***************************************************** #

.PHONY: debug debug.fsanitize debug.fs debug.pg

debug:
	$(eval DEBUGFLAGS=$(DEBUGFLAGS) -g3 -D DEBUG=1)
	@echo "\033[1;33m DEBUG MODE ACTIVATED $(_RESET)"

debug.fsanitize: debug
	$(eval DEBUGFLAGS=$(DEBUGFLAGS) -fsanitize=address)

debug.fs: debug.fsanitize

debug.pg:
	$(eval DEBUGFLAGS=$(DEBUGFLAGS) -pg)


# ***************************************************** #
# *                      Utils                        * #
# ***************************************************** #

.PHONY: header install uninstall update

_YELLOW	:= \033[1;33m
_GREEN	:= \033[1;32m
_RED	:= \033[1;31m
_RESET	:= \033[0m

header:
ifeq ($(MAKELEVEL), 0)
	@echo "Big Fat Header"
endif

# -----| Install / Uninstall |----- #

INSTALL_DIR = $(HOME)/.local/bin
install:
	mkdir -p $(INSTALL_DIR)
	cp $(NAME) $(INSTALL_DIR)/
	chmod +x $(INSTALL_DIR)/$(NAME)
	$(call _completion)
	echo "$(_GREEN) ✅ $(NAME) installed to $(INSTALL_DIR) $(_RESET)"; \

uninstall:
	rm -rf $(INSTALL_DIR)/$(NAME)
	echo "$(_GREEN) ✅ $(NAME) uninstalled from $(INSTALL_DIR) $(_RESET)";

update:
	if [ -f ./auto.sh ]; then \
		echo "\033[1;33m Updating Makefile... $(_RESET)"; \
	else \
		echo "$(_RED) auto.sh not found, please add the script to automate the update $(_RESET)"; \
		exit 1; \
	fi
	./auto.sh
	echo "$(_GREEN) ✅ Makefile updated $(_RESET)";
