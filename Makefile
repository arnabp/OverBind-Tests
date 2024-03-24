CC = gcc
CFLAGS = -IUnity-2.6.0 -DUNITY_SUPPORT_64 -IInterception-1.0.1 -g
LDFLAGS = -LInterception-1.0.1/x64 -linterception -luser32

TARGET = run_tests.exe
SOURCES = Unity-2.6.0/unity.c main.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

%.o: %.c
	@echo Compiling $<
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

test: $(TARGET)
	./$(TARGET)
