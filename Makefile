
CC = clang
#CC_FLAGS = -std=c11 -O2 -Iinclude -I/usr/include
CC_FLAGS = -Weverything -Wno-padded -std=c11 -O0 -g -Iinclude -I/usr/include
LD_FLAGS = -lSDL2 -lSDL2_gfx

#CC = gcc
#CC_FLAGS = -Wall -Wextra -std=c11 -O0 -g -Iinclude  -I/usr/include

EXEC = blackjack
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:.c=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) $(LD_FLAGS) -o $(EXEC)

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS) *~
