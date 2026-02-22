CC = gcc
CFLAGS = -Wall -Wextra -g

# The default target
all: memgrind

# Build memgrind from memgrind.c and mymalloc.c
memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) -o memgrind memgrind.o mymalloc.o

# Compile object files
memgrind.o: memgrind.c mymalloc.h
	$(CC) $(CFLAGS) -c memgrind.c

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c mymalloc.c

# Remove build artifacts
clean:
	rm -f *.o memgrind

