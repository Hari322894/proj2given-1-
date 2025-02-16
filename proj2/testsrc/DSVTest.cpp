#include "gtest/gtest.h"
#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"

TEST(CDSVReaderTest, ReadRowTest) {
    auto source = std::make_shared<CStringDataSource>("a,b,c\n1,2,3");
    CDSVReader reader(source, ',');
    std::vector<std::string> row;
    EXPECT_TRUE(reader.ReadRow(row));
    EXPECT_EQ(row, std::vector<std::string>({"a", "b", "c"}));
}

TEST(CDSVWriterTest, WriteRowTest) {
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');
    EXPECT_TRUE(writer.WriteRow({"a", "b", "c"}));
    EXPECT_EQ(sink->String(), "a,b,c\n");
}