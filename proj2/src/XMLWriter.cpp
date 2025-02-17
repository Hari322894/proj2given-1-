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
    
    SImplementation(std::shared_ptr<CDataSink> sink) : DataSink(std::move(sink)) {}
    
    // Helper function to escape special XML characters
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
            case SXMLEntity::EType::StartElement: {
                output = "<" + entity.DNameData;
                
                // Add attributes in sorted order
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">";
                break;
            }
            
            case SXMLEntity::EType::EndElement:
                output = "</" + entity.DNameData + ">";
                break;
                
            case SXMLEntity::EType::CharData:
                // Preserve exact character data including whitespace
                output = EscapeString(entity.DNameData);
                break;
                
            case SXMLEntity::EType::CompleteElement: {
                output = "<" + entity.DNameData;
                
                // Add attributes in sorted order
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                
                // Self-closing tag
                output += "/>";
                break;
            }
        }
        
        // Write the exact output to the data sink
        std::vector<char> outputChars(output.begin(), output.end());
        return DataSink->Write(outputChars);
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