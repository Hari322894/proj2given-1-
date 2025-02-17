#include "XMLReader.h"
#include <sstream>
#include <queue>

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

    void ParseAttributes(SXMLEntity& entity) {
        char ch;
        while (GetChar(ch)) {
            if (ch == '>' || ch == '/') {
                UngetChar(ch);
                break;
            }
            if (std::isspace(ch)) {
                continue;
            }

            // Read attribute name
            UngetChar(ch);
            std::string attrName = ReadUntil('=');
            
            // Get the = sign
            GetChar(ch); // =
            
            // Get opening quote
            GetChar(ch); // " or '
            char quote = ch;
            
            // Read attribute value
            std::string attrValue = ReadUntil(quote);
            GetChar(ch); // consume closing quote
            
            // Add attribute as a pair to the vector
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
            charData += ReadUntil('<');
            if (!skipcdata) {
                entity.DType = SXMLEntity::EType::CharData;
                entity.DNameData = charData;
                return true;
            }
            return ReadEntity(entity, skipcdata);
        }

        GetChar(ch);
        if (ch == '/') {
            // End element
            entity.DType = SXMLEntity::EType::EndElement;
            entity.DNameData = ReadUntil('>');
            GetChar(ch); // consume '>'
            return true;
        }

        // Start or complete element
        UngetChar(ch);
        entity.DNameData = ReadUntil(' ', true);
        if (entity.DNameData.back() == '>') {
            // Simple start element with no attributes
            entity.DNameData.pop_back();
            entity.DType = SXMLEntity::EType::StartElement;
            return true;
        }
        
        if (entity.DNameData.back() == '/') {
            // Self-closing element with no attributes
            entity.DNameData.pop_back();
            entity.DType = SXMLEntity::EType::CompleteElement;
            GetChar(ch); // consume '>'
            return true;
        }

        ParseAttributes(entity);
        
        GetChar(ch);
        if (ch == '/') {
            entity.DType = SXMLEntity::EType::CompleteElement;
            GetChar(ch); // consume '>'
        } else {
            UngetChar(ch);
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