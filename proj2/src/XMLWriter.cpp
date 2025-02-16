#include "XMLWriter.h"
#include <sstream>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;

    SImplementation(std::shared_ptr<CDataSink> sink)
        : DDataSink(sink) {}

    bool WriteEntity(const SXMLEntity &entity) {
        std::stringstream ss;
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                ss << "<" << entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    ss << " " << attr.first << "=\"" << attr.second << "\"";
                }
                ss << ">";
                break;
            case SXMLEntity::EType::EndElement:
                ss << "</" << entity.DNameData << ">";
                break;
            case SXMLEntity::EType::CompleteElement:
                ss << "<" << entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    ss << " " << attr.first << "=\"" << attr.second << "\"";
                }
                ss << "/>";
                break;
            case SXMLEntity::EType::CharData:
                ss << entity.DNameData;
                break;
        }
        std::string str = ss.str();
        DDataSink->Write(std::vector<char>(str.begin(), str.end()));
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(sink)) {}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}