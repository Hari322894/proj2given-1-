#include "XMLReader.h"

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    
    SImplementation(std::shared_ptr<CDataSource> src) : DataSource(std::move(src)) {}

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        // Simplified XML parsing for demonstration.
        if (DataSource->End()) return false;
        char ch;
        DataSource->Get(ch);
        entity.DType = SXMLEntity::EType::CharData;
        entity.DNameData += ch;
        return true;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(src)) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->DataSource->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}
