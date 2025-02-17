# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lgtest -lgtest_main -pthread

# Directories
SRC_DIR = src
TEST_DIR = testsrc
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Test object files - separate into GTest and standalone tests
GTEST_FILES = $(TEST_DIR)/StringUtilsTest.cpp $(TEST_DIR)/StringDataSourceTest.cpp $(TEST_DIR)/StringDataTest.cpp
STANDALONE_FILES = $(TEST_DIR)/XMLtest.cpp $(TEST_DIR)/DSVtest.cpp

GTEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(GTEST_FILES))
STANDALONE_OBJ_FILES = $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(STANDALONE_FILES))

# Output binaries
GTEST_TARGET = $(BIN_DIR)/runtests
XML_TARGET = $(BIN_DIR)/xmltest
DSV_TARGET = $(BIN_DIR)/dsvtest

# Default target
all: $(GTEST_TARGET) $(XML_TARGET) $(DSV_TARGET)

# Rule to build the Google Test binary
$(GTEST_TARGET): $(OBJ_FILES) $(GTEST_OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to build the XML test binary
$(XML_TARGET): $(OBJ_DIR)/XMLtest.o $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule to build the DSV test binary
$(DSV_TARGET): $(OBJ_DIR)/DSVtest.o $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule to compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run all tests
test: all
	./$(GTEST_TARGET)
	./$(XML_TARGET)
	./$(DSV_TARGET)

# Phony targets
.PHONY: all clean test