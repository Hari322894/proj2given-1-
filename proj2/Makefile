# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lexpat -lgtest -lgtest_main -pthread

# Directories
SRC_DIR = src
TEST_DIR = testsrc
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_FILES))

# Output binaries
TARGET = $(BIN_DIR)/testproj2
DRIVER = driver

# Default target
all: $(TARGET) $(DRIVER)

# Rule to build the test binary
$(TARGET): $(OBJ_FILES) $(TEST_OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to build the driver binary in the root directory
$(DRIVER): $(OBJ_FILES) driver.cpp
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) driver.cpp -o $@ $(LDFLAGS)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile test files into object files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DRIVER)

# Phony targets
.PHONY: all clean
