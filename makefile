# The language to compile
LANG=testlang

CC = gcc

CFLAGS = -Wall -Werror --pedantic -I./include/ -I./tests/ -O3

MAIN_SRCS=$(wildcard ./src/*.c)

LANG_SRCS=$(MAIN_SRCS) $(wildcard ./$(LANG)_files/*.c)
LANG_OBJS=$(LANG_SRCS:.c=.o)

TEST_SRCS=$(MAIN_SRCS) $(wildcard ./tests/*.c)
TEST_OBJS=$(TEST_SRCS:.c=.o)

TEST=test

test: $(TEST_OBJS)
	cc $^ -o $(TEST) $(CFLAGS) -I./tests/

test-clean:
	rm -f $(TEST_OBJS)

test-fclean:
	rm -f $(TEST_OBJS) $(TEST)

test-re: test-fclean test

lang: $(LANG_OBJS)
	cc $^ -o $(LANG) $(CFLAGS) -I./$(LANG)_files/

lang-clean:
	rm -f $(LANG_OBJS)

lang-fclean:
	rm -f $(LANG_OBJS) $(LANG)

lang-re: lang-fclean lang

