#include "DSVReader.h"
#include <sstream>
#include <iostream>

struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    char Delimiter;

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DataSource(std::move(src)), Delimiter(delimiter) {}

    bool ReadRow(std::vector<std::string> &row) {
        row.clear();
        std::string cell;
        char ch;
        bool inQuotes = false;
        bool hasData = false;
        bool preserveQuotes = false;

        while (!DataSource->End()) {
            if (!DataSource->Get(ch)) return false;
            hasData = true;

            if (ch == '"') {
                if (inQuotes) {
                    if (!DataSource->End()) {
                        char nextChar;
                        if (DataSource->Get(nextChar) && nextChar == '"') {
                            cell += '"';
                        } else {
                            inQuotes = false;
                            DataSource->PutBack(nextChar);
                        }
                    }
                } else {
                    inQuotes = true;
                    preserveQuotes = true;
                }
            } else if (ch == Delimiter && !inQuotes) {
                row.push_back(preserveQuotes ? '"' + cell + '"' : cell);
                cell.clear();
                preserveQuotes = false;
            } else if ((ch == '\n' || ch == '\r') && !inQuotes) {
                if (!cell.empty() || !row.empty()) {
                    row.push_back(preserveQuotes ? '"' + cell + '"' : cell);
                }
                return true;
            } else {
                cell += ch;
            }
        }

        if (!cell.empty() || hasData) {
            row.push_back(preserveQuotes ? '"' + cell + '"' : cell);
        }

        return hasData;
    }
};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {}

CDSVReader::~CDSVReader() = default;

bool CDSVReader::End() const {
    return DImplementation->DataSource->End();
}

bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}
