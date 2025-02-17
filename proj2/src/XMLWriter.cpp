#include "XMLWriter.h"
#include <sstream>
#include <unordered_map>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    // Special character map for XML escaping
    const std::unordered_map<char, std::string> EscapeMap = {
        {'&', "&amp;"},
        {'"', "&quot;"},
        {'\'', "&apos;"},
        {'<', "&lt;"},
        {'>', "&gt;"}
    };
    int IndentationLevel = 0;
    const std::string IndentationString = "\t";
    bool IsPreviousStartElement = false;

    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}

    // Helper function to escape special XML characters
    std::string EscapeString(const std::string &str) {
        std::string result;
        result.reserve(str.size() * 2); // Reserve extra space for potential escapes
        for (char c : str) {
            auto it = EscapeMap.find(c);
            if (it != EscapeMap.end()) {
                result += it->second;
            } else {
                result += c;
            }
        }
        return result;
    }

    // Helper function to add indentation
    void AddIndentation(std::string &output) {
        if (IsPreviousStartElement) {
            output += "\n" + std::string(IndentationLevel, '\t');
            IsPreviousStartElement = false;
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
                IsPreviousStartElement = true;
                break;
            case SXMLEntity::EType::EndElement:
                IndentationLevel--;
                AddIndentation(output);
                output += "</" + entity.DNameData + ">";
                break;
            case SXMLEntity::EType::CharData:
                AddIndentation(output);
                output += EscapeString(entity.DNameData);
                break;
            case SXMLEntity::EType::CompleteElement:
                AddIndentation(output);
                output += "<" + entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += "/>";
                break;
        }
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