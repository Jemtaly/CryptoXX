# Compiler configuration
CXX ?= g++
CXX_STANDARD = -std=c++20

# Build configuration
BUILD_TYPE ?= Release
BUILD_DIR = build
SAMPLES_DIR = samples
SCRIPTS_DIR = scripts

# Build type specific flags
ifeq ($(BUILD_TYPE),Debug)
    CXXFLAGS = $(CXX_STANDARD) -g -O0 -DDEBUG -Wall -Wextra -Wpedantic
else ifeq ($(BUILD_TYPE),RelWithDebInfo)
    CXXFLAGS = $(CXX_STANDARD) -g -O2 -DNDEBUG -Wall -Wextra
else ifeq ($(BUILD_TYPE),MinSizeRel)
    CXXFLAGS = $(CXX_STANDARD) -Os -DNDEBUG -Wall
else
    # Release (default)
    CXXFLAGS = $(CXX_STANDARD) -O3 -DNDEBUG -Wall
endif

# Add include directory and enable parallel compilation
CXXFLAGS += -I include
MAKEFLAGS += -j$(shell nproc 2>/dev/null || echo 4)

# Source and binary definitions
HASH_SRC = $(SAMPLES_DIR)/hash.cpp
HASH_BIN = $(BUILD_DIR)/hash
HASH_TEST = $(SCRIPTS_DIR)/hash_test.sh
CIPHER_SRC = $(SAMPLES_DIR)/cipher.cpp
CIPHER_BIN = $(BUILD_DIR)/cipher
CIPHER_TEST = $(SCRIPTS_DIR)/cipher_test.sh

# Find all header files for dependency tracking
HEADERS = $(shell find include -name "*.hpp" 2>/dev/null)
# Default target
.PHONY: all clean test install check format lint help debug release
.DEFAULT_GOAL := all

all: $(HASH_BIN) $(CIPHER_BIN)

# Build targets with proper dependency tracking
$(HASH_BIN): $(HASH_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(CIPHER_BIN): $(CIPHER_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Test targets
test: test-hash test-cipher

test-hash: $(HASH_BIN)
	@echo "Running hash tests..."
	@chmod +x $(HASH_TEST)
	@$(HASH_TEST) $(HASH_BIN)

test-cipher: $(CIPHER_BIN)
	@echo "Running cipher tests..."
	@chmod +x $(CIPHER_TEST)
	@$(CIPHER_TEST) $(CIPHER_BIN)

# Build type targets
debug:
	@$(MAKE) BUILD_TYPE=Debug

release:
	@$(MAKE) BUILD_TYPE=Release

relwithdebinfo:
	@$(MAKE) BUILD_TYPE=RelWithDebInfo

minsizerel:
	@$(MAKE) BUILD_TYPE=MinSizeRel

# Code quality targets
format:
	@echo "Formatting code with clang-format..."
	@find . -name "*.cpp" -o -name "*.hpp" | grep -E "(samples|include)" | xargs clang-format -i

check-format:
	@echo "Checking code formatting..."
	@find . -name "*.cpp" -o -name "*.hpp" | grep -E "(samples|include)" | xargs clang-format --dry-run --Werror

lint:
	@echo "Running static analysis with clang-tidy..."
	@if command -v clang-tidy >/dev/null 2>&1; then \
		find . -name "*.cpp" | grep samples | xargs clang-tidy -p=$(BUILD_DIR) --; \
	else \
		echo "clang-tidy not found, skipping lint check"; \
	fi

# Install target (basic)
install: all
	@echo "Installing binaries to /usr/local/bin..."
	@sudo cp $(HASH_BIN) $(CIPHER_BIN) /usr/local/bin/

# Clean target
clean:
	rm -rf $(BUILD_DIR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build all binaries (default)"
	@echo "  test         - Run all tests"
	@echo "  test-hash    - Run hash tests only"
	@echo "  test-cipher  - Run cipher tests only"
	@echo "  debug        - Build with debug flags"
	@echo "  release      - Build with release optimization"
	@echo "  format       - Format code with clang-format"
	@echo "  check-format - Check code formatting"
	@echo "  lint         - Run static analysis"
	@echo "  install      - Install binaries to system"
	@echo "  clean        - Remove build directory"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Build types (set BUILD_TYPE=):"
	@echo "  Debug        - Debug build with symbols"
	@echo "  Release      - Optimized release build (default)"
	@echo "  RelWithDebInfo - Release with debug info"
	@echo "  MinSizeRel   - Size-optimized release"
