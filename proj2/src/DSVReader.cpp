#include "DSVReader.h"
#include <sstream>

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

        while (!DataSource->End()) {
            if (!DataSource->Get(ch)) break;
            hasData = true;

            if (ch == '"') {
                if (inQuotes && !DataSource->End()) {
                    char nextChar;
                    if (DataSource->Get(nextChar)) {
                        if (nextChar == '"') {
                            // Escaped quote, add it to the cell
                            cell += '"';
                        } else {
                            // End of quoted section, keep the next character for processing
                            inQuotes = false;
                            DataSource->PutBack(nextChar);
                        }
                    }
                } else {
                    // Toggle quote mode if not handling escaped quotes
                    inQuotes = !inQuotes;
                }
            } else if (ch == Delimiter && !inQuotes) {
                // End of cell, add it to the row
                row.push_back(cell);
                cell.clear();
            } else {
                // Regular character, add to the cell
                cell += ch;
            }
        }

        // Add the last cell if any data was read
        if (!cell.empty() || hasData) {
            row.push_back(cell);
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
