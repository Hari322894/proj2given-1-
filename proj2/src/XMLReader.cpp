#include "XMLReader.h"
#include <sstream>
#include <queue>
#include <cctype>
#include <unordered_map>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    std::queue<char> Buffer;
    
    const std::unordered_map<std::string, char> EntityMap = {
        {"&amp;", '&'},
        {"&quot;", '"'},
        {"&apos;", '\''},
        {"&lt;", '<'},
        {"&gt;", '>'}
    };
    
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
    
    std::string DecodeEntities(const std::string& input) {
        std::string result;
        size_t pos = 0;
        
        while (pos < input.length()) {
            if (input[pos] == '&') {
                bool found = false;
                for (const auto& entity : EntityMap) {
                    if (input.substr(pos, entity.first.length()) == entity.first) {
                        result += entity.second;
                        pos += entity.first.length();
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    result += input[pos++];
                }
            } else {
                result += input[pos++];
            }
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
            
            // Decode entities in attribute value
            attrValue = DecodeEntities(attrValue);
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
            
            while (!charData.empty() && std::isspace(charData.back())) {
                charData.pop_back();
            }
            
            if (!skipcdata || !charData.empty()) {
                entity.DType = SXMLEntity::EType::CharData;
                entity.DNameData = DecodeEntities(charData);
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