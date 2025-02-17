#include "XMLWriter.h"
#include <vector>
#include <string>

class XMLWriter {
private:
    class Implementation {
    private:
        std::shared_ptr<DataSink> dataSink;
        std::vector<std::string> openElements;  // Using vector instead of stack

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
        explicit Implementation(std::shared_ptr<DataSink> sink) 
            : dataSink(std::move(sink)) {}

        bool flush() {
            // Close tags in reverse order
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

        bool writeEntity(const XMLEntity& entity) {
            switch (entity.type) {
                case XMLEntity::Type::StartElement:
                    if (!writeDirect("<") ||
                        !writeDirect(entity.name) ||
                        !writeAttributeList(entity.attributes) ||
                        !writeDirect(">")) {
                        return false;
                    }
                    openElements.push_back(entity.name);
                    return true;

                case XMLEntity::Type::EndElement:
                    if (!writeDirect("</") ||
                        !writeDirect(entity.name) ||
                        !writeDirect(">")) {
                        return false;
                    }
                    if (!openElements.empty()) {
                        openElements.pop_back();
                    }
                    return true;

                case XMLEntity::Type::CharData:
                    return writeEscapedContent(entity.name);

                case XMLEntity::Type::CompleteElement:
                    if (!writeDirect("<") ||
                        !writeDirect(entity.name) ||
                        !writeAttributeList(entity.attributes) ||
                        !writeDirect("/>")) {
                        return false;
                    }
                    return true;

                default:
                    return false;
            }
        }
    };

    std::unique_ptr<Implementation> impl;

public:
    explicit XMLWriter(std::shared_ptr<DataSink> sink)
        : impl(std::make_unique<Implementation>(std::move(sink))) {}

    ~XMLWriter() = default;

    bool flush() {
        return impl->flush();
    }

    bool writeEntity(const XMLEntity& entity) {
        return impl->writeEntity(entity);
    }
};