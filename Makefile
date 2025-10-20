CC = gcc
CFLAGS = -Wall
IMPORT_FLAGS = -I./ -I./lib/cJSON
# add -Wextra to flag
TEST = ./test/*.c
LIB_SRC = lib/cJSON/cJSON.c
SRC := $(wildcard ./*.c)
SRC := $(filter-out ./main.c, $(SRC))

# Build the main program
build: 
	$(CC) $(SRC) main.c $(LIB_SRC) $(CFLAGS) $(IMPORT_FLAGS) $^ -o game.out

# Debug
debug: 
	$(CC) $(SRC) main.c $(LIB_SRC) -g $(IMPORT_FLAGS) $^ -o main
# 	gdb game.out

# Build the tests and test
test: 
	$(CC) $(SRC) $(TEST) ./test/main.c $(LIB_SRC) $(CFLAGS) $(IMPORT_FLAGS) $^ -o test.out && ./test.out

# Build 
test-game: build 
	./game.out

clean:
	rm -f game test

