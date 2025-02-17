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

    // Flag to track if we are currently inside the root element
    bool insideRootElement = false;

    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;

        // Handle different entity types
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                output += "<" + entity.DNameData;
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">";
                if (entity.DNameData == "osm") {
                    insideRootElement = true; // Indicate we are inside the root element
                }
                break;

            case SXMLEntity::EType::EndElement:
                output += "</" + entity.DNameData + ">";
                if (entity.DNameData == "osm") {
                    insideRootElement = false; // Reset the flag when exiting the root element
                }
                break;

            case SXMLEntity::EType::CharData:
                output += EscapeString(entity.DNameData);
                break;

            case SXMLEntity::EType::CompleteElement:
                output += "<" + entity.DNameData;
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += "/>"; // Self-closing tag
                break;
        }

        // Write the output to the data sink
        bool success = DataSink->Write(std::vector<char>(output.begin(), output.end()));

        // If we are exiting the root element, write the closing tag
        if (!insideRootElement && entity.DType == SXMLEntity::EType::EndElement && entity.DNameData == "osm") {
            std::string closingTag = "\n</osm>\n"; // Ensure proper formatting
            success &= DataSink->Write(std::vector<char>(closingTag.begin(), closingTag.end()));
        }

        return success;
    }

    bool Flush() {
        return true; // No Flush method in CDataSink
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