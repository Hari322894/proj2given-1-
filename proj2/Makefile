CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lexpat  # For Expat library

# Directories
OBJDIR = obj
BINDIR = bin

# Source and object files
SOURCES = src/DSVReader.cpp src/DSVWriter.cpp src/StringDataSink.cpp src/StringDataSource.cpp src/XMLReader.cpp src/XMLWriter.cpp
TEST_SOURCES = testsrc/DSVTest.cpp testsrc/StringDataSinkTest.cpp testsrc/StringDataSourceTest.cpp testsrc/XMLTest.cpp

OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:%.cpp=$(OBJDIR)/%.o)

# Executables
EXECUTABLES = $(BINDIR)/testdsv $(BINDIR)/testxml

# Targets
all: directories $(EXECUTABLES)

# Create directories
directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

# Compile source files
$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test source files
$(OBJDIR)/%.o: testsrc/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files to create executables
$(BINDIR)/testdsv: $(OBJDIR)/DSVReader.o $(OBJDIR)/DSVWriter.o $(OBJDIR)/DSVTest.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BINDIR)/testxml: $(OBJDIR)/XMLReader.o $(OBJDIR)/XMLWriter.o $(OBJDIR)/XMLTest.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Run tests
test: $(EXECUTABLES)
	$(BINDIR)/testdsv
	$(BINDIR)/testxml

# Clean up
clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

.PHONY: all test clean directories