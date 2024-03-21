CC = gcc
CFLAGS = -IUnity-2.6.0 -IInterception-1.0.1

# Test executable name
TEST_BIN = run_tests

# Source files
UNITY_SRC = Unity-2.6.0/unity.c
INTERCEPTION_SRC = Interception-1.0.1/interception.c
MAIN_SRC = main.c

# Object files
OBJS = $(UNITY_SRC:.c=.o) $(INTERCEPTION_SRC:.c=.o) $(MAIN_SRC:.c=.o)

.PHONY: all clean test

all: $(TEST_BIN)

$(TEST_BIN): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJS) $(TEST_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)
