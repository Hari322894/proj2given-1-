#include "XMLWriter.h"

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;

    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}

    bool WriteEntity(const SXMLEntity &entity) {
        DataSink->Write(std::vector<char>(entity.DNameData.begin(), entity.DNameData.end()));
        return true;
    }

    bool Flush() {
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(sink)) {}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}
