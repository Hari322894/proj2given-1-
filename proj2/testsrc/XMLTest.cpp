#include "gtest/gtest.h"
#include "XMLReader.h"
#include "XMLWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"

TEST(CXMLReaderTest, ReadEntityTest) {
    auto source = std::make_shared<CStringDataSource>("<root><child/></root>");
    CXMLReader reader(source);
    SXMLEntity entity;
    EXPECT_TRUE(reader.ReadEntity(entity, false));
    EXPECT_EQ(entity.DType, SXMLEntity::EType::StartElement);
    EXPECT_EQ(entity.DNameData, "root");
}

TEST(CXMLWriterTest, WriteEntityTest) {
    auto sink = std::make_shared<CStringDataSink>();
    CXMLWriter writer(sink);
    SXMLEntity entity;
    entity.DType = SXMLEntity::EType::StartElement;
    entity.DNameData = "root";
    EXPECT_TRUE(writer.WriteEntity(entity));
    EXPECT_EQ(sink->String(), "<root>");
}