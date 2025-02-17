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
                // Handle escaped quotes (e.g., "My name is ""Bob""!")
                if (inQuotes && !DataSource->End()) {
                    char nextChar;
                    if (DataSource->Get(nextChar)) {
                        if (nextChar == '"') {
                            cell += '"';  // Add escaped quote
                        } else {
                            DataSource->Unget(nextChar);  // Unget the character
                            inQuotes = !inQuotes;  // Toggle quote mode
                        }
                    }
                } else {
                    inQuotes = !inQuotes;  // Toggle quote mode
                }
            } else if (ch == Delimiter && !inQuotes) {
                row.push_back(cell);  // Push cell when delimiter is hit outside quotes
                cell.clear();
            } else {
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
