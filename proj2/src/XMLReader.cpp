#include "XMLReader.h"
#include <expat.h>
#include <cstring>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DDataSource;
    XML_Parser DParser;
    SXMLEntity DCurrentEntity;
    bool DEndReached;

    SImplementation(std::shared_ptr<CDataSource> src)
        : DDataSource(src), DEndReached(false) {
        DParser = XML_ParserCreate(nullptr);
        XML_SetUserData(DParser, this);
        XML_SetElementHandler(DParser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(DParser, CharDataHandler);
    }

    ~SImplementation() {
        XML_ParserFree(DParser);
    }

    static void StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts) {
        auto impl = static_cast<SImplementation *>(userData);
        impl->DCurrentEntity.DType = SXMLEntity::EType::StartElement;
        impl->DCurrentEntity.DNameData = name;
        impl->DCurrentEntity.DAttributes.clear();
        for (int i = 0; atts[i]; i += 2) {
            impl->DCurrentEntity.DAttributes.emplace_back(atts[i], atts[i + 1]);
        }
    }

    static void EndElementHandler(void *userData, const XML_Char *name) {
        auto impl = static_cast<SImplementation *>(userData);
        impl->DCurrentEntity.DType = SXMLEntity::EType::EndElement;
        impl->DCurrentEntity.DNameData = name;
    }

    static void CharDataHandler(void *userData, const XML_Char *s, int len) {
        auto impl = static_cast<SImplementation *>(userData);
        impl->DCurrentEntity.DType = SXMLEntity::EType::CharData;
        impl->DCurrentEntity.DNameData = std::string(s, len);
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        std::vector<char> buffer(1024);
        while (!DEndReached) {
            if (DDataSource->Read(buffer, buffer.size())) {
                if (!XML_Parse(DParser, buffer.data(), buffer.size(), false)) {
                    DEndReached = true;
                    return false;
                }
                if (DCurrentEntity.DType != SXMLEntity::EType::CharData || !skipcdata) {
                    entity = DCurrentEntity;
                    return true;
                }
            } else {
                DEndReached = true;
                XML_Parse(DParser, nullptr, 0, true);
            }
        }
        return false;
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(src)) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->DEndReached;
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}