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
        bool preserveQuotes = false;  // Track if field was fully quoted

        while (!DataSource->End()) {
            if (!DataSource->Get(ch)) return false;
            hasData = true;

            if (ch == '"') {
                if (inQuotes) {
                    // Check if the next character is another quote (escaped quote)
                    if (!DataSource->End()) {
                        char nextChar;
                        if (DataSource->Get(nextChar) && nextChar == '"') {
                            cell += '"';  // Escaped quote, add it
                        } else {
                            inQuotes = false; // End of quoted section
                            if (nextChar == Delimiter) {
                                row.push_back("\"" + cell + "\""); // Preserve full quotes
                                cell.clear();
                            } else if (nextChar == '\n' || nextChar == '\r') {
                                row.push_back("\"" + cell + "\"");
                                return true;
                            } else {
                                preserveQuotes = true;
                                cell += nextChar;
                            }
                        }
                    }
                } else {
                    inQuotes = true;
                    preserveQuotes = true;
                }
            } else if (ch == Delimiter && !inQuotes) {
                if (preserveQuotes) {
                    row.push_back("\"" + cell + "\"");
                } else {
                    row.push_back(cell);
                }
                cell.clear();
                preserveQuotes = false;
            } else if ((ch == '\n' || ch == '\r') && !inQuotes) {
                if (!cell.empty() || !row.empty()) {
                    if (preserveQuotes) {
                        row.push_back("\"" + cell + "\"");
                    } else {
                        row.push_back(cell);
                    }
                }
                return true;
            } else {
                cell += ch;
            }
        }

        // Add the last cell if there was any data
        if (!cell.empty() || hasData) {
            if (preserveQuotes) {
                row.push_back("\"" + cell + "\"");
            } else {
                row.push_back(cell);
            }
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
