CC = g++
CFLAGS = -Wall
LDFLAGS =
OBJFILES = $(wildcard *.cpp)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f *.o