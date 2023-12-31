CC = gcc
CFLAGS = -W -Wall -Wextra -Wno-missing-braces -std=c99 -fPIE -flto -Wno-digraphs -Os
LDFLAGS = -flto

ifeq ($(OS),Windows_NT)
	EXECUTABLE = DZ8x.exe
	SHELL = cmd.exe
	RM = del /f 2>nul
else
	EXECUTABLE = DZ8x
	RM = rm -f
endif

SOURCES = lodepng.c replace.c main.c

OBJECTS := $(SOURCES:.c=.o)

all: $(EXECUTABLE) cleano

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(EXECUTABLE) $(EXECUTABLE) $(OBJECTS)

cleano:
	$(RM) $(OBJECTS)

.PHONY: clean
