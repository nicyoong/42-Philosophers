NAME      = philo
CC        = cc
CFLAGS    = -Wall -Wextra -Werror
SRCS      = philo.c \
            philocore.c \
            philoerror.c \
            philoinit.c \
            philolife2.c \
            philolife.c \
            philomonitor2.c \
            philomonitor.c \
            philoutils.c

OBJS      = $(SRCS:.c=.o)

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c philo.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all