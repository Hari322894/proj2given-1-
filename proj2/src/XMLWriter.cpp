#include "XMLWriter.h"
#include <sstream>
#include <unordered_map>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    bool HasRootElement = false;
    
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
            if (it != EscapeMap.end()) {
                result += it->second;
            } else {
                result += c;
            }
        }
        return result;
    }
    
    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;
        
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                if (!HasRootElement) {
                    HasRootElement = true;
                    output = "<" + entity.DNameData;
                } else {
                    output = "\n\t\n\t<" + entity.DNameData;  // Double newline before node
                }
                
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">";
                break;
                
            case SXMLEntity::EType::EndElement:
                if (entity.DNameData != "osm") {  // Skip osm closing tag
                    output = "</" + entity.DNameData + ">";
                }
                break;
                
            case SXMLEntity::EType::CharData:
                output = EscapeString(entity.DNameData);
                break;
                
            case SXMLEntity::EType::CompleteElement:
                if (!HasRootElement) {
                    HasRootElement = true;
                    output = "<" + entity.DNameData;
                } else {
                    output = "\n\t\n\t<" + entity.DNameData;  // Double newline before node
                }
                
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += "/>";
                break;
        }
        
        if (!output.empty()) {
            return DataSink->Write(std::vector<char>(output.begin(), output.end()));
        }
        return true;
    }
    
    bool Flush() {
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(sink)) {}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}