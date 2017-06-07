# The language to compile
LANG=firstlang

CC = gcc

CFLAGS = -Wall -Werror --pedantic -I. -lm -g

SRCS=$(wildcard ./*.c) $(wildcard ./$(LANG)/*.c)
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
