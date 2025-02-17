#include "XMLWriter.h"
#include <sstream>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    bool StartTagWritten = false;
    
    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}
    
    std::string EscapeString(const std::string &str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&apos;"; break;
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                default: result += c;
            }
        }
        return result;
    }

    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;
        
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                if (!StartTagWritten && entity.DNameData == "osm") {
                    output = "<" + entity.DNameData;
                    for (const auto &attr : entity.DAttributes) {
                        output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                    }
                    output += ">\n\n\t\t";
                    StartTagWritten = true;
                } else {
                    output = "<" + entity.DNameData;
                    for (const auto &attr : entity.DAttributes) {
                        output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                    }
                    output += ">";
                }
                break;
                
            case SXMLEntity::EType::EndElement:
                output = "</" + entity.DNameData + ">";
                break;
                
            case SXMLEntity::EType::CharData:
                output = EscapeString(entity.DNameData);
                break;
                
            case SXMLEntity::EType::CompleteElement:
                if (StartTagWritten && entity.DNameData == "node") {
                    output = "<" + entity.DNameData;
                    for (const auto &attr : entity.DAttributes) {
                        output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                    }
                    output += "/>\n\n\t\t";
                } else {
                    output = "<" + entity.DNameData;
                    for (const auto &attr : entity.DAttributes) {
                        output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                    }
                    output += "/>";
                }
                break;
        }
        
        return DataSink->Write(std::vector<char>(output.begin(), output.end()));
    }

    bool Flush() {
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(std::move(sink))) {}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}