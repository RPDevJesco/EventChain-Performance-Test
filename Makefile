# EventChains Makefile
# Alternative to Docker-based builds for local development

CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wextra -pedantic
LDFLAGS = -lm

# Source files
SOURCES = eventchains.c multi_tier_benchmark.c
HEADERS = eventchains.h
OBJECTS = $(SOURCES:.c=.o)
TARGET = eventchain_test

# Dijkstra benchmark
DIJKSTRA_SOURCES = eventchains.c dijkstra_benchmark.c
DIJKSTRA_OBJECTS = $(DIJKSTRA_SOURCES:.c=.o)
DIJKSTRA_TARGET = dijkstra_benchmark

# Test suite
TEST_SOURCES = eventchains.c test_main.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_TARGET = test_suite

.PHONY: all clean test dijkstra help

# Default target
all: $(TARGET)

# Main benchmark
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	strip $@

# Dijkstra benchmark
dijkstra: $(DIJKSTRA_TARGET)

$(DIJKSTRA_TARGET): $(DIJKSTRA_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	strip $@

# Test suite
test: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	strip $@

# Compile source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Run benchmarks
run: $(TARGET)
	./$(TARGET) 10000

run-dijkstra: dijkstra
	./$(DIJKSTRA_TARGET)

run-test: test
	./$(TEST_TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(DIJKSTRA_OBJECTS) $(TEST_OBJECTS)
	rm -f $(TARGET) $(DIJKSTRA_TARGET) $(TEST_TARGET)
	rm -rf build-output/

# Docker build
docker-build:
	./build.sh

# Help
help:
	@echo "EventChains Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all              Build main benchmark (default)"
	@echo "  dijkstra         Build Dijkstra benchmark"
	@echo "  test             Build test suite"
	@echo "  run              Build and run main benchmark"
	@echo "  run-dijkstra     Build and run Dijkstra benchmark"
	@echo "  run-test         Build and run test suite"
	@echo "  clean            Remove all build artifacts"
	@echo "  docker-build     Build using Docker for cross-platform"
	@echo "  help             Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Build main benchmark"
	@echo "  make run          # Build and run with 10000 iterations"
	@echo "  make dijkstra     # Build Dijkstra algorithm benchmark"
	@echo "  make clean        # Clean all build artifacts"
