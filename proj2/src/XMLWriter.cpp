#include "XMLWriter.h"
#include <sstream>
#include <unordered_map>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    bool isFirstElement = true;
    bool isRootElement = true;
   
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
   
    bool WriteEntity(const SXMLEntity &entity) {
        std::string output;

        // Handle different entity types
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                if (!isFirstElement && entity.DNameData != "osm") {
                    output = "\n\t\n\t\n\t";
                }
                output += "<" + entity.DNameData;
               
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
                output += ">";
                isRootElement = (entity.DNameData == "osm");
                break;
               
            case SXMLEntity::EType::EndElement:
                if (entity.DNameData == "osm") {
                    output = "\n\t\n</osm>";
                } else {
                    output = "</" + entity.DNameData + ">";
                }
                break;
               
            case SXMLEntity::EType::CharData:
                output = EscapeString(entity.DNameData);
                break;
               
            case SXMLEntity::EType::CompleteElement:
                if (!isFirstElement) {
                    output = "\n\t\n\t\n\t";
                }
                output += "<" + entity.DNameData;
               
                // Add attributes
                for (const auto &attr : entity.DAttributes) {
                    output += " " + attr.first + "=\"" + EscapeString(attr.second) + "\"";
                }
               
                // Self-closing tag
                output += "/>";
                break;
        }
       
        if (isFirstElement) {
            isFirstElement = false;
        }

        // Write the output to the data sink
        return DataSink->Write(std::vector<char>(output.begin(), output.end()));
    }
   
    bool Flush() {
        return true; // No Flush() method in CDataSink
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