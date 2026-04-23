NAME = codexion

SRC = atoi.c coder_routine.c get_time.c init.c main.c monitor_routine.c \
	parsing.c print.c heap.c

OBJDIR = .obj
OBJ = $(SRC:%.c=$(OBJDIR)/%.o)
DEP = $(OBJ:%.o=%.d)

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread  -I ./ -MMD -MP

#-fsanitize=thread

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
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
