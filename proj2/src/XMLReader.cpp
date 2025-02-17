#include "XMLReader.h"
#include <expat.h>
#include <cstring>  // For std::strlen

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    XML_Parser Parser;
    SXMLEntity CurrentEntity;
    bool HasEntity;
    bool EndOfData;

    SImplementation(std::shared_ptr<CDataSource> src)
        : DataSource(std::move(src)), Parser(XML_ParserCreate(nullptr)), HasEntity(false), EndOfData(false) {
        XML_SetUserData(Parser, this);
        XML_SetElementHandler(Parser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(Parser, CharacterDataHandler);
    }

    ~SImplementation() {
        XML_ParserFree(Parser);
    }

    static void StartElementHandler(void* userData, const char* name, const char** attrs) {
        auto* impl = static_cast<SImplementation*>(userData);
        impl->CurrentEntity.DType = SXMLEntity::EType::StartElement;
        impl->CurrentEntity.DNameData = name;
        
        impl->CurrentEntity.DAttributes.clear();
        for (int i = 0; attrs[i]; i += 2) {
            impl->CurrentEntity.DAttributes.emplace_back(attrs[i], attrs[i + 1]);
        }

        impl->HasEntity = true;
    }

    static void EndElementHandler(void* userData, const char* name) {
        auto* impl = static_cast<SImplementation*>(userData);
        impl->CurrentEntity.DType = SXMLEntity::EType::EndElement;
        impl->CurrentEntity.DNameData = name;
        impl->HasEntity = true;
    }

    static void CharacterDataHandler(void* userData, const char* data, int len) {
        auto* impl = static_cast<SImplementation*>(userData);
        impl->CurrentEntity.DType = SXMLEntity::EType::CharData;
        impl->CurrentEntity.DNameData.assign(data, len);
        impl->HasEntity = true;
    }

    bool ReadEntity(SXMLEntity& entity, bool skipcdata) {
        HasEntity = false;
        std::vector<char> buffer(4096);  // Use a vector instead of a C-style array
    
        while (!EndOfData && !HasEntity) {
            size_t bytesRead = DataSource->Read(buffer, buffer.size());
            if (bytesRead == 0) {
                EndOfData = true;
                XML_Parse(Parser, nullptr, 0, XML_TRUE);  // Signal end of parsing
                break;
            }
    
            // Pass the data from the vector to the parser
            if (!XML_Parse(Parser, buffer.data(), bytesRead, XML_FALSE)) {
                return false;  // Parsing error
            }
        }
    
        if (HasEntity) {
            entity = CurrentEntity;
            return true;
        }
    
        return false;
    }
};
