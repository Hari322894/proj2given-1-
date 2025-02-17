#include "XMLWriter.h"
#include <memory>
#include <unordered_map>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    int IndentationLevel = 0;
    bool PrettyPrint = true;
    
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
        
        // Add newline before elements except for the first one
        if (PrettyPrint && entity.DType != SXMLEntity::EType::CharData && IndentationLevel > 0) {
            output += "\n";
        }
        
        // Add indentation for all types except CharData
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
                if (PrettyPrint) {
                    IndentationLevel--;
                }
                output += "</" + entity.DNameData + ">";
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
        
        // Add newline after elements except CharData and CompleteElement
        if (PrettyPrint && 
            entity.DType != SXMLEntity::EType::CharData && 
            entity.DType != SXMLEntity::EType::CompleteElement) {
            output += "\n";
        }
        
        return DataSink->Write(std::vector<uint8_t>(output.begin(), output.end()));
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