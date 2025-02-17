#include "XMLWriter.h"
#include <sstream>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    int IndentationLevel = 0;
    const std::string IndentationString = "\t";

    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}

    std::string EscapeString(const std::string &str) {
        std::string result;
        result.reserve(str.size() * 2); // Reserve extra space for potential escapes
        for (char c : str) {
            switch (c) {
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&apos;"; break;
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                default: result += c; break;
            }
        }
        return result;
    }

    void AddIndentation(std::string &output) {
        if (!output.empty()) {
            output += "\n" + std::string(IndentationLevel, '\t');
        }
    }

    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                AddIndentation(output);
                output += "<" + entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">";
                IndentationLevel++;
                break;

            case SXMLEntity::EType::EndElement:
                IndentationLevel--;
                AddIndentation(output);
                output += "</" + entity.DNameData + ">";
                break;

            case SXMLEntity::EType::CharData:
                output += EscapeString(entity.DNameData);
                break;

            case SXMLEntity::EType::CompleteElement:
                AddIndentation(output);
                output += "<" + entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += "/>"; // Self-closing tag
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