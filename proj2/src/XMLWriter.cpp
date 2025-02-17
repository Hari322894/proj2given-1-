#include "XMLWriter.h"
#include <vector>
#include <string>

class CXMLWriter {
private:
    class Implementation {
    private:
        std::shared_ptr<CDataSink> dataSink;
        std::vector<std::string> openElements;

        bool writeDirect(const std::string& str) {
            for (char c : str) {
                if (!dataSink->Put(c)) {
                    return false;
                }
            }
            return true;
        }

        bool writeEscapedContent(const std::string& content) {
            for (char c : content) {
                bool success = true;
                switch (c) {
                    case '<':  success = writeDirect("&lt;"); break;
                    case '>':  success = writeDirect("&gt;"); break;
                    case '&':  success = writeDirect("&amp;"); break;
                    case '\'': success = writeDirect("&apos;"); break;
                    case '"':  success = writeDirect("&quot;"); break;
                    default:   success = dataSink->Put(c); break;
                }
                if (!success) return false;
            }
            return true;
        }

        bool writeAttributeList(const std::vector<std::pair<std::string, std::string>>& attributes) {
            for (const auto& attr : attributes) {
                if (!writeDirect(" ") ||
                    !writeDirect(attr.first) ||
                    !writeDirect("=\"") ||
                    !writeEscapedContent(attr.second) ||
                    !writeDirect("\"")) {
                    return false;
                }
            }
            return true;
        }

    public:
        explicit Implementation(std::shared_ptr<CDataSink> sink) 
            : dataSink(std::move(sink)) {}

        bool flush() {
            for (auto it = openElements.rbegin(); it != openElements.rend(); ++it) {
                if (!writeDirect("</") ||
                    !writeDirect(*it) ||
                    !writeDirect(">")) {
                    return false;
                }
            }
            openElements.clear();
            return true;
        }

        bool writeEntity(const SXMLEntity& entity) {
            switch (entity.DType) {
                case SXMLEntity::EType::StartElement:
                    if (!writeDirect("<") ||
                        !writeDirect(entity.DNameData) ||
                        !writeAttributeList(entity.DAttributes) ||
                        !writeDirect(">")) {
                        return false;
                    }
                    openElements.push_back(entity.DNameData);
                    return true;

                case SXMLEntity::EType::EndElement:
                    if (!writeDirect("</") ||
                        !writeDirect(entity.DNameData) ||
                        !writeDirect(">")) {
                        return false;
                    }
                    if (!openElements.empty()) {
                        openElements.pop_back();
                    }
                    return true;

                case SXMLEntity::EType::CharData:
                    return writeEscapedContent(entity.DNameData);

                case SXMLEntity::EType::CompleteElement:
                    if (!writeDirect("<") ||
                        !writeDirect(entity.DNameData) ||
                        !writeAttributeList(entity.DAttributes) ||
                        !writeDirect("/>")) {
                        return false;
                    }
                    return true;
            }
            return false;
        }
    };

    std::unique_ptr<Implementation> impl;

public:
    explicit CXMLWriter(std::shared_ptr<CDataSink> sink)
        : impl(std::make_unique<Implementation>(std::move(sink))) {}

    ~CXMLWriter() = default;

    bool Flush() {
        return impl->flush();
    }

    bool WriteEntity(const SXMLEntity& entity) {
        return impl->writeEntity(entity);
    }
};