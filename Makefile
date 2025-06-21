CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -O2
# Math library linking (only needed on Unix-like systems)
ifeq ($(OS),Windows_NT)
    LDFLAGS =
else
    LDFLAGS = -lm
endif
TARGET = archimed
TEST_TARGET = test_archimed
SOURCES = main.c block.c pi.c utils.c
TEST_SOURCES = test.c block.c pi.c utils.c
OBJECTS = $(SOURCES:.c=.o)
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Test executable
$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_TARGET) $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TEST_OBJECTS) $(TARGET) $(TEST_TARGET) $(TARGET).exe $(TEST_TARGET).exe

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# For Windows
run-win: $(TARGET).exe
	$(TARGET).exe

test-win: $(TEST_TARGET).exe
	$(TEST_TARGET).exe

# Create Windows executable
$(TARGET).exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET).exe $(LDFLAGS)

$(TEST_TARGET).exe: $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $(TEST_TARGET).exe $(LDFLAGS)

.PHONY: all clean run test run-win test-win
