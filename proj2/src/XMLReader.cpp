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
                return;
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
        while (true) {
            SkipWhitespace();
            if (!GetChar(ch)) {
                break;
            }
    
            if (ch == '>' || ch == '/') {
                UngetChar(ch);
                break;
            }
    
            UngetChar(ch);
            std::string attrName = ReadTagName();
            if (attrName.empty()) {
                break;
            }
    
            SkipWhitespace();
            if (!GetChar(ch) || ch != '=') {
                UngetChar(ch);
                entity.DAttributes.push_back(std::make_pair(attrName, ""));
                continue;
            }
    
            SkipWhitespace();
            if (!GetChar(ch)) {
                break;
            }
    
            std::string attrValue;
            if (ch == '"' || ch == '\'') {
                char quote = ch;
                while (GetChar(ch) && ch != quote) {
                    attrValue += ch;
                }
            } else {
                UngetChar(ch);
                while (GetChar(ch) && !std::isspace(ch) && ch != '>' && ch != '/') {
                    attrValue += ch;
                }
                if (ch == '>' || ch == '/') {
                    UngetChar(ch);
                }
            }
    
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
            std::string charData;
            charData += ch;
            while (GetChar(ch) && ch != '<') {
                charData += ch;
            }
            if (ch == '<') {
                UngetChar(ch);
            }
    
            if (skipcdata) {
                return ReadEntity(entity, skipcdata);
            }
    
            entity.DType = SXMLEntity::EType::CharData;
            entity.DNameData = DecodeEntities(charData);
            std::cout << "CharData: " << entity.DNameData << std::endl;
            return true;
        }
    
        if (!GetChar(ch)) {
            return false;
        }
    
        if (ch == '/') {
            entity.DType = SXMLEntity::EType::EndElement;
            entity.DNameData = ReadTagName();
            while (GetChar(ch) && ch != '>') {}
            std::cout << "EndElement: " << entity.DNameData << std::endl;
            return true;
        }
    
        if (ch == '!') {
            std::string specialTag;
            for (int i = 0; i < 2; ++i) {
                if (GetChar(ch)) {
                    specialTag += ch;
                }
            }
    
            if (specialTag == "--") {
                while (GetChar(ch) && !(ch == '-' && GetChar(ch) && ch == '-' && GetChar(ch) && ch == '>')) {}
                return ReadEntity(entity, skipcdata);
            }
    
            while (GetChar(ch) && ch != '>') {}
            return ReadEntity(entity, skipcdata);
        }
    
        if (ch == '?') {
            while (GetChar(ch) && !(ch == '?' && GetChar(ch) && ch == '>')) {}
            return ReadEntity(entity, skipcdata);
        }
    
        UngetChar(ch);
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = ReadTagName();
        std::cout << "StartElement: " << entity.DNameData << std::endl;
        ParseAttributes(entity);
    
        if (!GetChar(ch)) {
            return false;
        }
    
        if (ch == '/') {
            entity.DType = SXMLEntity::EType::CompleteElement;
            GetChar(ch);  // Consume '>'
        } else if (ch != '>') {
            UngetChar(ch);
        }
    
        std::cout << "ElementType: " << (entity.DType == SXMLEntity::EType::CompleteElement ? "CompleteElement" : "StartElement") << std::endl;
        for (const auto& attr : entity.DAttributes) {
            std::cout << "Attribute: " << attr.first << " = " << attr.second << std::endl;
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