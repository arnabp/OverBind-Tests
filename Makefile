CC = cl
CFLAGS = /I "Unity-2.6.0" /I "Interception-1.0.1"
LDFLAGS = /link /LIBPATH:"Interception-1.0.1/x86" interception.lib User32.lib

TARGET = run_tests.exe
SOURCES = Unity-2.6.0\unity.c main.c
OBJECTS = Unity-2.6.0\unity.obj main.obj

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) /Fe$(TARGET) $(CFLAGS) $(LDFLAGS)

{Unity-2.6.0}.c.obj:
	echo Compiling $<
	$(CC) $(CFLAGS) /c $< /Fo$@

%.obj: %.c
	echo Compiling $<
	$(CC) $(CFLAGS) /c $< /Fo$@

clean:
	del $(OBJECTS) $(TARGET)

test: $(TARGET)
	$(TARGET)