#include "DSVReader.h"
#include <sstream>
#include <algorithm>

struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> DDataSource;
    char DDelimiter;
    bool DEndReached;

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DDataSource(src), DDelimiter(delimiter), DEndReached(false) {}

    bool ReadRow(std::vector<std::string> &row) {
        row.clear();
        std::string cell;
        char ch;
        bool inQuotes = false;
        bool escapeNext = false;

        while (DDataSource->Get(ch)) {
            if (escapeNext) {
                cell += ch;
                escapeNext = false;
            } else if (ch == '"') {
                inQuotes = !inQuotes;
            } else if (ch == '\\') {
                escapeNext = true;
            } else if (!inQuotes && ch == DDelimiter) {
                row.push_back(cell);
                cell.clear();
            } else if (!inQuotes && (ch == '\n' || ch == '\r')) {
                if (!cell.empty() || !row.empty()) {
                    row.push_back(cell);
                }
                return true;
            } else {
                cell += ch;
            }
        }

        if (!cell.empty() || !row.empty()) {
            row.push_back(cell);
        }
        DEndReached = true;
        return !row.empty();
    }
};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {}

CDSVReader::~CDSVReader() = default;

bool CDSVReader::End() const {
    return DImplementation->DEndReached;
}

bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}