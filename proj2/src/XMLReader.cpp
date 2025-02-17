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
            
            if (ch == '>' || ch == '/') {  // End of attributes
                UngetChar(ch);
                break;
            }
            
            UngetChar(ch);  // Return character to buffer to parse attribute name
            std::string attrName = ReadTagName();
            if (attrName.empty()) {
                break;
            }
            
            SkipWhitespace();
            if (!GetChar(ch) || ch != '=') {  // Expect '=' after attribute name
                break;
            }
            
            SkipWhitespace();
            if (!GetChar(ch) || (ch != '"' && ch != '\'')) {  // Attribute value must be enclosed in quotes
                break;
            }
    
            char quote = ch;
            std::string attrValue;
            while (GetChar(ch) && ch != quote) {  // Read attribute value until matching quote
                attrValue += ch;
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
    
        if (ch != '<') {  // Character data
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
            return true;
        }
    
        // Start of tag
        GetChar(ch);
        if (ch == '/') {  // End element
            entity.DType = SXMLEntity::EType::EndElement;
            entity.DNameData = ReadTagName();
            while (GetChar(ch) && ch != '>') {}
            return true;
        }
    
        if (ch == '!') {  // Handle comments and other special tags
            std::string specialTag;
            for (int i = 0; i < 2; ++i) {
                if (GetChar(ch)) {
                    specialTag += ch;
                }
            }
    
            if (specialTag == "--") {  // Comment
                while (GetChar(ch) && !(ch == '-' && GetChar(ch) && ch == '-' && GetChar(ch) && ch == '>')) {}
                return ReadEntity(entity, skipcdata);
            }
    
            // Skip other special tags like `<!DOCTYPE>` or `<![CDATA[`
            while (GetChar(ch) && ch != '>') {}
            return ReadEntity(entity, skipcdata);
        }
    
        if (ch == '?') {  // XML declaration or processing instruction
            while (GetChar(ch) && !(ch == '?' && GetChar(ch) && ch == '>')) {}
            return ReadEntity(entity, skipcdata);
        }
    
        UngetChar(ch);
        entity.DNameData = ReadTagName();
        ParseAttributes(entity);  // Parse any attributes
    
        if (!GetChar(ch)) {
            return false;
        }
    
        if (ch == '/') {  // Self-closing tag
            entity.DType = SXMLEntity::EType::CompleteElement;
            GetChar(ch);  // Consume '>'
        } else if (ch == '>') {  // Regular start tag
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