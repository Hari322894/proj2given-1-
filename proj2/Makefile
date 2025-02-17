CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -g -Iinclude

SRC = src/DSVReader.cpp src/DSVWriter.cpp src/StringDataSink.cpp \
      src/StringDataSource.cpp src/StringUtils.cpp src/XMLReader.cpp src/XMLWriter.cpp

TEST_SRC = testsrc/DSVTest.cpp testsrc/StringDataSinkTest.cpp \
           testsrc/StringDataSourceTest.cpp testsrc/StringUtilsTest.cpp testsrc/XMLTest.cpp

OBJS = $(SRC:.cpp=.o)
TEST_OBJS = $(TEST_SRC:.cpp=.o)
HEADERS = include/DataSink.h include/DataSource.h include/DSVReader.h include/DSVWriter.h \
          include/StringDataSink.h include/StringDataSource.h include/StringUtils.h \
          include/XMLEntity.h include/XMLReader.h include/XMLWriter.h

TARGET = parser_app
TEST_TARGET = test_runner

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJS) $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean run test