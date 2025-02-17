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
                if (inQuotes) {
                    // Check if the next character is another quote
                    if (!DataSource->End()) {
                        char nextChar;
                        if (DataSource->Get(nextChar)) {
                            if (nextChar == '"') {
                                // Escaped quote, add a single quote
                                cell += '"';
                            } else if (nextChar == Delimiter) {
                                // End of quoted section, add cell to row
                                row.push_back(cell);
                                cell.clear();
                                inQuotes = false;
                            } else {
                                // Any other character, add both quote and char
                                cell += '"';
                                cell += nextChar;
                            }
                        }
                    }
                } else {
                    // Starting a quoted section
                    inQuotes = true;
                }
            } else if (ch == Delimiter && !inQuotes) {
                // Add the cell to the row when hitting a delimiter outside quotes
                row.push_back(cell);
                cell.clear();
            } else if (ch == '\n' && !inQuotes) {
                // End of the line, finalize row
                row.push_back(cell);
                return true;
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
