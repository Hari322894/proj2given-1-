#include "XMLWriter.h"
#include <sstream>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    int IndentLevel;

    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)), IndentLevel(0) {}

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

    std::string GetIndentation() const {
        return std::string(IndentLevel, '\t'); // Use tab characters for indentation
    }

    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;
        // Handle different entity types
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                output = GetIndentation() + "<" + entity.DNameData;
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">\n";
                IndentLevel++;
                break;
            case SXMLEntity::EType::EndElement:
                IndentLevel--;
                output = GetIndentation() + "</" + entity.DNameData + ">\n";
                break;
            case SXMLEntity::EType::CharData:
                output = GetIndentation() + EscapeString(entity.DNameData) + "\n";
                break;
            case SXMLEntity::EType::CompleteElement:
                output = GetIndentation() + "<" + entity.DNameData;
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                // Self-closing tag
                output += "/>\n";
                break;
        }
        // Write the output to the data sink
        return DataSink->Write(std::vector<char>(output.begin(), output.end()));
    }

    bool Flush() {
        return true; // No Flush() method in CDataSink
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