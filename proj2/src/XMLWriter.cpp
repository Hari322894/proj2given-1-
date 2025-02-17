#include "XMLWriter.h"
#include <sstream>
#include <unordered_map>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    int IndentationLevel = 0;
    bool PrettyPrint = true; // Ensures correct formatting with newlines and tabs

    const std::unordered_map<char, std::string> EscapeMap = {
        {'&', "&amp;"},
        {'"', "&quot;"},
        {'\'', "&apos;"},
        {'<', "&lt;"},
        {'>', "&gt;"}
    };

    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}

    std::string EscapeString(const std::string &str) {
        std::string result;
        result.reserve(str.size() * 2);
        for (char c : str) {
            auto it = EscapeMap.find(c);
            result += (it != EscapeMap.end()) ? it->second : std::string(1, c);
        }
        return result;
    }

    std::string GetIndentation() {
        return PrettyPrint ? std::string(IndentationLevel, '\t') : "";
    }

    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;
    
        // Do not indent CharData
        if (PrettyPrint && entity.DType != SXMLEntity::EType::CharData) {
            output += GetIndentation();
        }
    
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                output += "<" + entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">";
                if (PrettyPrint) {
                    IndentationLevel++;
                }
                break;
    
            case SXMLEntity::EType::EndElement:
                IndentationLevel--;
                if (PrettyPrint) {
                    output = GetIndentation() + "</" + entity.DNameData + ">";
                } else {
                    output += "</" + entity.DNameData + ">";
                }
                break;
    
            case SXMLEntity::EType::CharData:
                output += EscapeString(entity.DNameData);
                break;
    
            case SXMLEntity::EType::CompleteElement:
                output += "<" + entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += "/>";
                break;
        }
    
        // Only add newline for StartElement and EndElement (not for CompleteElement)
        if (PrettyPrint && entity.DType != SXMLEntity::EType::CompleteElement) {
            output += "\n";
        }
    
        return DataSink->Write(std::vector<char>(output.begin(), output.end()));
    }
    

    bool Flush() {
        return true; // No special flushing required for CDataSink
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
