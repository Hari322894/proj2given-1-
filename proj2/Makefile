# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -pthread
LDFLAGS = -pthread -lexpat  # Add -lexpat here

# Directories
SRC_DIR = src
TEST_DIR = testsrc
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include
GTEST_DIR = googletest
GTEST_BUILD_DIR = $(GTEST_DIR)/build
GTEST_LIB = $(GTEST_BUILD_DIR)/lib/libgtest.a
GTEST_MAIN_LIB = $(GTEST_BUILD_DIR)/lib/libgtest_main.a

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

# Download and build GoogleTest using CMake
setup_gtest:
	@if [ ! -d "$(GTEST_DIR)" ]; then \
		echo "Cloning GoogleTest..."; \
		git clone --depth=1 https://github.com/google/googletest.git $(GTEST_DIR); \
	fi
	@mkdir -p $(GTEST_BUILD_DIR)
	@cd $(GTEST_BUILD_DIR) && cmake .. && make -j

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -isystem $(GTEST_DIR)/googletest/include -c $< -o $@

# Link test executables with GoogleTest and Expat
$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(OBJ_FILES) $(GTEST_LIB) $(GTEST_MAIN_LIB)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)  # Ensure LDFLAGS is used here

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