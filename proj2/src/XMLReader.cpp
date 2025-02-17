#include "XMLReader.h"
#include <sstream>
#include <queue>
#include <cctype>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    std::queue<char> Buffer;
    
    SImplementation(std::shared_ptr<CDataSource> src) : DataSource(std::move(src)) {}

    bool GetChar(char& ch) {
        if (!Buffer.empty()) {
            ch = Buffer.front();
            Buffer.pop();
            return true;
        }
        return DataSource->Get(ch);
    }

    void UngetChar(char ch) {
        Buffer.push(ch);
    }

    void SkipWhitespace() {
        char ch;
        while (GetChar(ch)) {
            if (!std::isspace(ch)) {
                UngetChar(ch);
                break;
            }
        }
    }

    std::string ReadUntil(char stopChar, bool includeStop = false) {
        std::string result;
        char ch;
        while (GetChar(ch)) {
            if (ch == stopChar) {
                if (includeStop) {
                    result += ch;
                } else {
                    UngetChar(ch);
                }
                break;
            }
            result += ch;
        }
        return result;
    }

    std::string ReadTagName() {
        std::string result;
        char ch;
        while (GetChar(ch)) {
            if (std::isspace(ch) || ch == '>' || ch == '/' || ch == '=') {
                UngetChar(ch);
                break;
            }
            result += ch;
        }
        return result;
    }

    void ParseAttributes(SXMLEntity& entity) {
        char ch;
        SkipWhitespace();
        while (GetChar(ch)) {
            if (ch == '>' || ch == '/') {
                UngetChar(ch);
                break;
            }
            if (std::isspace(ch)) {
                continue;
            }

            UngetChar(ch);
            std::string attrName = ReadTagName();
            
            SkipWhitespace();
            GetChar(ch); // =
            SkipWhitespace();
            
            GetChar(ch); // " or '
            char quote = ch;
            
            std::string attrValue;
            while (GetChar(ch) && ch != quote) {
                attrValue += ch;
            }
            
            entity.DAttributes.push_back(std::make_pair(attrName, attrValue));
        }
    }

    bool ReadEntity(SXMLEntity& entity, bool skipcdata) {
        entity.DAttributes.clear();
        entity.DNameData.clear();
        
        char ch;
        SkipWhitespace();
        
        if (!GetChar(ch)) {
            return false;
        }

        if (ch != '<') {
            // Character data
            std::string charData;
            charData += ch;
            while (GetChar(ch) && ch != '<') {
                charData += ch;
            }
            if (ch == '<') {
                UngetChar(ch);
            }
            
            // Trim trailing whitespace from charData
            while (!charData.empty() && std::isspace(charData.back())) {
                charData.pop_back();
            }
            
            if (!skipcdata || !charData.empty()) {
                entity.DType = SXMLEntity::EType::CharData;
                entity.DNameData = charData;
                return true;
            }
            return ReadEntity(entity, skipcdata);
        }

        // Handle special tags
        GetChar(ch);
        if (ch == '?') {
            while (GetChar(ch) && !(ch == '?' && GetChar(ch) && ch == '>')) {}
            return ReadEntity(entity, skipcdata);
        }

        if (ch == '/') {
            // End element
            entity.DType = SXMLEntity::EType::EndElement;
            entity.DNameData = ReadTagName();
            // Skip to and consume '>'
            while (GetChar(ch) && ch != '>') {}
            return true;
        }

        // Start or complete element
        UngetChar(ch);
        entity.DNameData = ReadTagName();
        
        SkipWhitespace();
        ParseAttributes(entity);
        
        GetChar(ch);
        if (ch == '/') {
            entity.DType = SXMLEntity::EType::CompleteElement;
            GetChar(ch); // consume '>'
        } else if (ch == '>') {
            entity.DType = SXMLEntity::EType::StartElement;
        }

        return true;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(src)) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->DataSource->End();
}

bool CXMLReader::ReadEntity(SXMLEntity& entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}