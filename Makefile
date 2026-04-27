NAME = codexion

SRCDIR = coders
SRC = atoi.c coder_routine.c coder_utils.c get_time.c init.c main.c \
	monitor_routine.c parsing.c print.c heap.c heap_utils.c \
	dongle_init_utils.c init_rollback.c setup.c

OBJDIR = .obj
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
DEP = $(OBJ:%.o=%.d)

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -g3 -I $(SRCDIR) -MMD -MP

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

-include $(DEP)