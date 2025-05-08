# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g

# Source and object files
SRC = server.c request.c response.c
OBJ = $(SRC:.c=.o)

# Output executable name
EXEC = http_server

# Default target
all: $(EXEC)

# Link the object files to create the executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean up build artifacts
clean:
	rm -f $(OBJ) $(EXEC)