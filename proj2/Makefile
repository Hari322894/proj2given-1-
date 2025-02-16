# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -pthread

# Directories
SRC_DIR = src
TEST_DIR = testsrc
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include
GTEST_DIR = googletest
GTEST_LIB = $(OBJ_DIR)/libgtest.a

# Find all source and test files
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

# Convert source files to object files
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
TEST_OBJ_FILES := $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_FILES))

# Convert test object files to executables
TEST_EXECUTABLES := $(patsubst $(OBJ_DIR)/%.o, $(BIN_DIR)/%, $(TEST_OBJ_FILES))

# Default target
all: setup_gtest directories $(TEST_EXECUTABLES)

# Download and build GoogleTest
setup_gtest:
	@mkdir -p $(OBJ_DIR)
	@if [ ! -d "$(GTEST_DIR)" ]; then \
		echo "Cloning GoogleTest..."; \
		git clone https://github.com/google/googletest.git $(GTEST_DIR); \
	fi
	@$(CXX) $(CXXFLAGS) -isystem $(GTEST_DIR)/googletest/include -pthread -c $(GTEST_DIR)/googletest/src/gtest-all.cc -o $(OBJ_DIR)/gtest-all.o
	@ar rcs $(GTEST_LIB) $(OBJ_DIR)/gtest-all.o

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -isystem $(GTEST_DIR)/googletest/include -c $< -o $@

# Link test executables
$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(OBJ_FILES) $(GTEST_LIB)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# Create directories if not present
directories:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

# Run tests
test: all
	@echo "Running tests..."
	@for test in $(TEST_EXECUTABLES); do $$test; done

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(GTEST_DIR)

.PHONY: all directories test clean
