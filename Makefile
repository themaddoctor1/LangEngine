# The language to compile
LANG=testlang

CC = gcc

CFLAGS = -Wall -Werror --pedantic -I. -lm -g

MAIN_SRCS=$(wildcard ./src/*.c)

LANG_SRCS=$(MAIN_SRCS) $(wildcard ./$(LANG)_files/*.c)
LANG_OBJS=$(LANG_SRCS:.c=.o)

TEST_SRCS=$(MAIN_SRCS) $(wildcard ./tests/*.c)
TEST_OBJS=$(TEST_SRCS:.c=.o)

TEST=test

show-files:
	echo "lang objs:" $(LANG_OBJS)
	echo "test objs:" $(TEST_OBJS)

test: $(TEST_OBJS)
	cc $^ -o $(TEST) $(CFLAGS)

test-clean:
	rm -f $(TEST_OBJS)

test-fclean:
	rm -f $(TEST_OBJS) $(TEST)

lang: $(LANG_OBJS)
	cc $^ -o $(LANG) $(CFLAGS)

lang-clean:
	rm -f $(LANG_OBJS)

lang-fclean:
	rm -f $(LANG_OBJS) $(LANG)

