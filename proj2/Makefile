CXX = g++
CXXFLAGS = -std=c++17 -Wall
OBJDIR = obj
BINDIR = bin

# Source and object files
SOURCES = src/DSVReader.cpp src/DSVWriter.cpp src/StringDataSink.cpp src/StringDataSource.cpp src/StringUtils.cpp src/XMLReader.cpp src/XMLWriter.cpp
TEST_SOURCES = testsrc/DSVTest.cpp testsrc/StringDataSinkTest.cpp testsrc/StringDataSourceTest.cpp testsrc/StringUtilsTest.cpp testsrc/XMLTest.cpp

OBJECTS = $(SOURCES:%.cpp=$(OBJDIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:%.cpp=$(OBJDIR)/%.o)

# Executables
EXECUTABLES = $(BINDIR)/teststrutils $(BINDIR)/teststrdatasource $(BINDIR)/teststrdatasink $(BINDIR)/testdsv $(BINDIR)/testxml

# Targets
all: directories $(EXECUTABLES)

runtests: $(EXECUTABLES)
	$(EXECUTABLES)

$(BINDIR)/teststrutils: $(OBJDIR)/StringUtils.o $(OBJDIR)/StringUtilsTest.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BINDIR)/teststrdatasource: $(OBJDIR)/StringDataSource.o $(OBJDIR)/StringDataSourceTest.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BINDIR)/teststrdatasink: $(OBJDIR)/StringDataSink.o $(OBJDIR)/StringDataSinkTest.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BINDIR)/testdsv: $(OBJDIR)/DSVReader.o $(OBJDIR)/DSVWriter.o $(OBJDIR)/DSVTest.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BINDIR)/testxml: $(OBJDIR)/XMLReader.o $(OBJDIR)/XMLWriter.o $(OBJDIR)/XMLTest.o
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: testsrc/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(EXECUTABLES)
	$(BINDIR)/teststrutils
	$(BINDIR)/teststrdatasource
	$(BINDIR)/teststrdatasink
	$(BINDIR)/testdsv
	$(BINDIR)/testxml

directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)