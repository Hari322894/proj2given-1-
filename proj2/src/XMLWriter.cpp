#include "XMLWriter.h"
#include <vector>
#include <string>

struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    std::vector<std::string> DElementList;  // Using vector instead of stack
    
    SImplementation(std::shared_ptr<CDataSink> sink)
        : DDataSink(sink) {}
    
    bool WriteString(const std::string& str) {
        for (char ch : str) {
            if (!DDataSink->Put(ch)) {
                return false;
            }
        }
        return true;
    }
    
    bool WriteEscaped(const std::string& str) {
        for (char ch : str) {
            switch (ch) {
                case '<':
                    if (!WriteString("&lt;")) return false;
                    break;
                case '>':
                    if (!WriteString("&gt;")) return false;
                    break;
                case '&':
                    if (!WriteString("&amp;")) return false;
                    break;
                case '\'':
                    if (!WriteString("&apos;")) return false;
                    break;
                case '"':
                    if (!WriteString("&quot;")) return false;
                    break;
                default:
                    if (!DDataSink->Put(ch)) return false;
            }
        }
        return true;
    }
    
    bool Flush() {
        for (auto it = DElementList.rbegin(); it != DElementList.rend(); ++it) {
            if (!WriteString("</") ||
                !WriteString(*it) ||
                !WriteString(">")) {
                return false;
            }
        }
        DElementList.clear();
        return true;
    }
    
    bool WriteEntity(const SXMLEntity& entity) {
        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                if (!WriteString("<") ||
                    !WriteString(entity.DNameData)) {
                    return false;
                }
                for (const auto& attr : entity.DAttributes) {
                    if (!WriteString(" ") ||
                        !WriteString(attr.first) ||
                        !WriteString("=\"") ||
                        !WriteEscaped(attr.second) ||
                        !WriteString("\"")) {
                        return false;
                    }
                }
                if (!WriteString(">")) {
                    return false;
                }
                DElementList.push_back(entity.DNameData);
                break;
                
            case SXMLEntity::EType::EndElement:
                if (!WriteString("</") ||
                    !WriteString(entity.DNameData) ||
                    !WriteString(">")) {
                    return false;
                }
                if (!DElementList.empty()) {
                    DElementList.pop_back();
                }
                break;
                
            case SXMLEntity::EType::CharData:
                if (!WriteEscaped(entity.DNameData)) {
                    return false;
                }
                break;
                
            case SXMLEntity::EType::CompleteElement:
                if (!WriteString("<") ||
                    !WriteString(entity.DNameData)) {
                    return false;
                }
                for (const auto& attr : entity.DAttributes) {
                    if (!WriteString(" ") ||
                        !WriteString(attr.first) ||
                        !WriteString("=\"") ||
                        !WriteEscaped(attr.second) ||
                        !WriteString("\"")) {
                        return false;
                    }
                }
                if (!WriteString("/>")) {
                    return false;
                }
                break;
        }
        return true;
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(sink)) {
}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}

bool CXMLWriter::WriteEntity(const SXMLEntity& entity) {
    return DImplementation->WriteEntity(entity);
}