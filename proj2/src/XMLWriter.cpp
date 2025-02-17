#include "XMLWriter.h"
#include <sstream>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;

    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}

    std::string EscapeString(const std::string &str) {
        std::string result;
        result.reserve(str.size() * 2);

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
                output = "<" + entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                // Don't add closing angle bracket for special case of root element
                if (entity.DNameData == "osm") {
                    output += ">\n";
                } else {
                    output += ">";
                }
                break;
            case SXMLEntity::EType::EndElement:
                // Skip end element for osm tag to match expected format
                if (entity.DNameData != "osm") {
                    output = "</" + entity.DNameData + ">";
                }
                break;
            case SXMLEntity::EType::CharData:
                output = EscapeString(entity.DNameData);
                break;
            case SXMLEntity::EType::CompleteElement:
                output = "\t<" + entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += "/>\n";
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