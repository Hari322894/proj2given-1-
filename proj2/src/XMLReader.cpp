#include "XMLReader.h"
#include <expat.h>
#include <queue>
#include <memory>
#include <vector>

//implementing details of XML Reader into struct function
struct CXMLReader::SImplementation {
    //shared pointer to datasource in order for reading
    std::shared_ptr<CDataSource> DataSource;
    //expat parser for XML parsing(included the expat.h)
    XML_Parser Parser;
    //queue to store XML entities as they are parsed
    std::queue<SXMLEntity> EntityQueue;
    //flag to track if we've reached end of data
    bool EndOfData;
    //buffer to accumulate character data between tags
    std::string CharDataBuffer;

    //handler for start element tags in XML
    static void StartElementHandler(void *userData, const char *name, const char **atts) {
        //cast the user data back to our implementation
        auto *impl = static_cast<SImplementation *>(userData);
        //flush any pending character data before handling new element
        impl->FlushCharData();
        
        //create new entity for start element
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::StartElement;
        entity.DNameData = name;

        //parse attributes if they exist
        if (atts != nullptr) {
            for (int i = 0; atts[i] != nullptr; i += 2) {
                if (atts[i + 1] != nullptr) {
                    //add attribute name-value pair to entity
                    entity.DAttributes.emplace_back(atts[i], atts[i + 1]);
                }
            }
        }

        //add entity to our processing queue
        impl->EntityQueue.push(entity);
    }

    //handler for end element tags in XML
    static void EndElementHandler(void *userData, const char *name) {
        //cast the user data back to our implementation
        auto *impl = static_cast<SImplementation *>(userData);
        //flush any pending character data before handling end element
        impl->FlushCharData();
        
        //create new entity for end element
        SXMLEntity entity;
        entity.DType = SXMLEntity::EType::EndElement;
        entity.DNameData = name;
        //add entity to our processing queue
        impl->EntityQueue.push(entity);
    }

    //handler for character data between XML tags
    static void CharDataHandler(void *userData, const char *s, int len) {
        //cast the user data back to our implementation
        auto *impl = static_cast<SImplementation *>(userData);
        //append character data to buffer if valid
        if (s != nullptr && len > 0) {
            impl->CharDataBuffer.append(s, len);
        }
    }

    //initialize implementation with data source
    SImplementation(std::shared_ptr<CDataSource> src) : DataSource(std::move(src)), EndOfData(false) {
        //create XML parser
        Parser = XML_ParserCreate(nullptr);
        //set up parser handlers
        XML_SetUserData(Parser, this);
        XML_SetElementHandler(Parser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(Parser, CharDataHandler);
    }

    //cleanup parser on destruction
    ~SImplementation() {
        XML_ParserFree(Parser);
    }

    //flush accumulated character data to entity queue
    void FlushCharData() {
        if (!CharDataBuffer.empty()) {
            //create new entity for character data
            SXMLEntity entity;
            entity.DType = SXMLEntity::EType::CharData;
            entity.DNameData = CharDataBuffer;
            //add to queue and clear buffer
            EntityQueue.push(entity);
            CharDataBuffer.clear();
        }
    }

    //read next entity from XML, optionally skipping character data
    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        //keep reading until we have entities or reach end
        while (EntityQueue.empty() && !EndOfData) {
            //buffer for reading chunks of data
            std::vector<char> buffer(4096);
            size_t length = 0;

            //read data into buffer
            while (length < buffer.size() && !DataSource->End()) {
                char ch;
                if (DataSource->Get(ch)) {
                    buffer[length++] = ch;
                } else {
                    break;
                }
            }

            //check if we've reached end of data
            if (length == 0) {
                EndOfData = true;
                //signal end of parsing to expat
                XML_Parse(Parser, nullptr, 0, 1);
                break;
            }

            //parse the buffer of data
            if (XML_Parse(Parser, buffer.data(), length, 0) == XML_STATUS_ERROR) {
                return false;
            }
        }

        //return next entity if available
        if (!EntityQueue.empty()) {
            entity = EntityQueue.front();
            EntityQueue.pop();

            //skip character data if requested
            if (skipcdata && entity.DType == SXMLEntity::EType::CharData) {
                return ReadEntity(entity, skipcdata);
            }

            return true;
        }

        return false;
    }
};

//constructor for XML Reader
CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(std::move(src))) {}

//destructor for XML Reader
CXMLReader::~CXMLReader() = default;

//check if we've reached the end of XML data
bool CXMLReader::End() const {
    return DImplementation->EndOfData && DImplementation->EntityQueue.empty();
}

//read next entity from XML
bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}