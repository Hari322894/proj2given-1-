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
            if (!DataSource->Get(ch)) return false;
            hasData = true;

            if (ch == '"') {
                if (inQuotes && !DataSource->End()) {
                    char nextChar;
                    if (DataSource->Get(nextChar)) {
                        if (nextChar == '"') {
                            // Escaped quote, add a single quote to cell
                            cell += '"';
                        } else {
                            // End of quoted section, keep the next character for processing
                            inQuotes = false;
                            cell += ch;  // Add the current quote
                            cell += nextChar;
                        }
                    }
                } else {
                    // Toggle quote mode if not handling escaped quotes
                    inQuotes = !inQuotes;
                    cell += ch;
                }
            } else if (ch == Delimiter && !inQuotes) {
                // Add the cell to the row when hitting a delimiter outside quotes
                row.push_back(cell);
                cell.clear();
            } else {
                // Regular character, just add to the cell
                cell += ch;
            }
        }

        // Add the last cell if there was any data
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
