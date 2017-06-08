# The language to compile
LANG=firstlang

CC = gcc

CFLAGS = -Wall -Werror --pedantic -I. -lm -g



LANG_SRCS=$(wildcard ./*.c) $(wildcard ./$(LANG)/*.c)
LANG_OBJS=$(LANG_SRCS:.c=.o)

TEST_SRCS=$(wildcard ./*.c) $(wildcard ./test/*.c)
TEST_OBJS=$(TEST_SRCS:.c=.o)

EXEC=exec

show-files:
	echo "lang objs:" $(LANG_OBJS)
	echo "test objs:" $(TEST_OBJS)

test: $(TEST_OBJS)
	cc $^ -o $(EXEC) $(CFLAGS)

test-clean:
	rm -f $(TEST_OBJS)

lang: $(LANG_OBJS)
	cc $^ -o $(LANG) $(CFLAGS)

lang-clean:
	rm -f $(LANG_OBJS)

