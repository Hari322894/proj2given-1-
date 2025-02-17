#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <memory>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    XML_Parser Parser;
    std::queue<SXMLEntity> EntityQueue;

    static void StartElementHandler(void *userData, const char *name, const char **atts) {
        auto *impl = static_cast<SImplementation *>(userData);
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;

        // Parse attributes
        if (atts != nullptr) {
            for (int i = 0; atts[i] != nullptr; i += 2) {
                if (atts[i + 1] != nullptr) {
                    entity.DAttributes.emplace_back(atts[i], atts[i + 1]);
                }
            }
        }

        impl->EntityQueue.push(entity);
    }

    static void EndElementHandler(void *userData, const char *name) {
        auto *impl = static_cast<SImplementation *>(userData);
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;
        impl->EntityQueue.push(entity);
    }

    static void CharDataHandler(void *userData, const char *s, int len) {
        auto *impl = static_cast<SImplementation *>(userData);
        if (s != nullptr && len > 0) {
            std::string text(s, len);

            if (!text.empty()) {
                SXMLEntity entity;
                entity.DType = SXMLEntity::EType::CharData;
                entity.DNameData = text;
                impl->EntityQueue.push(entity);
            }
        }
    }

    SImplementation(std::shared_ptr<CDataSource> src) : DataSource(std::move(src)) {
        Parser = XML_ParserCreate(nullptr);
        XML_SetUserData(Parser, this);
        XML_SetElementHandler(Parser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(Parser, CharDataHandler);
    }

    ~SImplementation() {
        XML_ParserFree(Parser);
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        while (EntityQueue.empty()) {
            std::vector<char> buffer(4096); // Use vector instead of char array
            size_t length = DataSource->Read(buffer, buffer.size());

            if (length == 0) {
                // No more data to read
                break;
            }

            if (XML_Parse(Parser, buffer.data(), length, length == 0) == XML_STATUS_ERROR) {
                // Parsing error
                return false;
            }
        }

        if (!EntityQueue.empty()) {
            entity = EntityQueue.front();
            EntityQueue.pop();

            // Skip CDATA if requested
            if (skipcdata && entity.DType == SXMLEntity::EType::CharData) {
                return ReadEntity(entity, skipcdata); // Recursively skip CDATA
            }

            return true;
        }

        return false; // No more entities to read
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(std::move(src))) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->DataSource->End() && DImplementation->EntityQueue.empty();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}
