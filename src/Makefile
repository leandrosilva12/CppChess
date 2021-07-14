CC = g++
CFLAGS = -Wall
LDFLAGS = -lm
OBJFILES = $(wildcard *.cpp)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f *.o