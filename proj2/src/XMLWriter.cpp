#include "XMLWriter.h"
#include <sstream>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;

    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}

    // Helper function to escape special XML characters
    std::string EscapeString(const std::string &str) {
        std::string result;
        result.reserve(str.size() * 2); // Reserve extra space for potential escapes

        for (char c : str) {
            switch (c) {
                case '&':
                    result += "&amp;";
                    break;
                case '"':
                    result += "&quot;";
                    break;
                case '\'':
                    result += "&apos;";
                    break;
                case '<':
                    result += "&lt;";
                    break;
                case '>':
                    result += "&gt;";
                    break;
                default:
                    result += c; // Add the character as is
            }
        }
        return result;
    }

    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;

        // Handle different entity types
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                output = "<" + entity.DNameData;
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">";
                break;
            case SXMLEntity::EType::EndElement:
                output = "</" + entity.DNameData + ">";
                break;
            case SXMLEntity::EType::CharData:
                output = EscapeString(entity.DNameData);
                break;
            case SXMLEntity::EType::CompleteElement:
                output = "<" + entity.DNameData;
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                // Self-closing tag
                output += "/>";
                break;
        }

        // Write the output to the data sink
        bool success = DataSink->Write(std::vector<char>(output.begin(), output.end()));

        // If this is the root element, write the closing tag
        if (entity.DType == SXMLEntity::EType::StartElement && entity.DNameData == "osm") {
            std::string closingTag = "\n</osm>";
            success &= DataSink->Write(std::vector<char>(closingTag.begin(), closingTag.end()));
        }

        return success;
    }

    bool Flush() {
        return DataSink->Flush(); // Call Flush() on the data sink if available
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