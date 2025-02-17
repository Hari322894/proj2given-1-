#include "XMLReader.h"
#include "XMLWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <gtest/gtest.h>

TEST(XMLTest, BasicReadWrite) {
    std::shared_ptr<CStringDataSource> src = std::make_shared<CStringDataSource>("<tag>data</tag>");
    std::shared_ptr<CStringDataSink> sink = std::make_shared<CStringDataSink>();

    CXMLReader reader(src);
    CXMLWriter writer(sink);

    SXMLEntity entity;
    while (!reader.End()) {
        if (reader.ReadEntity(entity)) {
            writer.WriteEntity(entity);
        }
    }

    EXPECT_EQ(sink->String(), "<tag>data</tag>"); 
}


