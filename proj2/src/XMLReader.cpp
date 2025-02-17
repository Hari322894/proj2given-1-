#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <memory>
#include <vector>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    XML_Parser Parser;
    std::queue<SXMLEntity> EntityQueue;
    bool EndOfData;
    std::string CharDataBuffer;

    static void StartElementHandler(void *userData, const char *name, const char **atts) {
        auto *impl = static_cast<SImplementation *>(userData);
        impl->FlushCharData();
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
        impl->FlushCharData();
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;
        impl->EntityQueue.push(entity);
    }

    static void CharDataHandler(void *userData, const char *s, int len) {
        auto *impl = static_cast<SImplementation *>(userData);
        if (s != nullptr && len > 0) {
            impl->CharDataBuffer.append(s, len);
        }
    }

    SImplementation(std::shared_ptr<CDataSource> src) : DataSource(std::move(src)), EndOfData(false) {
        Parser = XML_ParserCreate(nullptr);
        XML_SetUserData(Parser, this);
        XML_SetElementHandler(Parser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(Parser, CharDataHandler);
    }

    ~SImplementation() {
        XML_ParserFree(Parser);
    }

    void FlushCharData() {
        if (!CharDataBuffer.empty()) {
            SXMLEntity entity;
            entity.DType = SXMLEntity::EType::CharData;
            entity.DNameData = CharDataBuffer;
            EntityQueue.push(entity);
            CharDataBuffer.clear();
        }
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        while (EntityQueue.empty() && !EndOfData) {
            std::vector<char> buffer(4096); // Use vector instead of char array
            size_t length = 0;
            while (length < buffer.size() && !DataSource->End()) {
                char ch;
                if (DataSource->Get(ch)) {
                    buffer[length++] = ch;
                } else {
                    break;
                }
            }

            if (length == 0) {
                // No more data to read
                EndOfData = true;
                XML_Parse(Parser, nullptr, 0, 1); // Signal end of parsing
                break;
            }

            if (XML_Parse(Parser, buffer.data(), length, 0) == XML_STATUS_ERROR) {
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
    return DImplementation->EndOfData && DImplementation->EntityQueue.empty();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}