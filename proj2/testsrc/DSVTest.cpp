#include "DSVReader.h"
#include "DSVWriter.h"
#include "StringDataSource.h"
#include "StringDataSink.h"
#include <iostream>

int main() {
    std::shared_ptr<CStringDataSource> src = std::make_shared<CStringDataSource>("a,b,c\n1,2,3\n");
    std::shared_ptr<CStringDataSink> sink = std::make_shared<CStringDataSink>();

    CDSVReader reader(src, ',');
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row;
    while (!reader.End()) {
        if (reader.ReadRow(row)) {
            writer.WriteRow(row);
        }
    }

    std::cout << "Written data: " << sink->String() << std::endl;
    return 0;
}
