
CC = gcc

CFLAGS = -Wall -Werror --pedantic -Iinclude -lm -g

SRCS=$(wildcard ./*.c)
OBJS=$(SRCS:.c=.o)

EXEC=exec

all: $(OBJS)
	$(CC) -o $(EXEC) $^ $(CFLAGS)

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(OBJS) $(EXEC)

re:
	make fclean all
