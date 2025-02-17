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
            bool escapedQuote = false;
        
            while (!DataSource->End()) {
                if (!DataSource->Get(ch)) {
                    // End of source reached
                    if (!cell.empty() || !row.empty() || inQuotes) {
                        row.push_back(cell);
                    }
                    return !row.empty();
                }
        
                if (ch == '"') {
                    if (inQuotes && !escapedQuote) {
                        // Check if this is an escaped quote
                        if (DataSource->Peek() == '"') {
                            escapedQuote = true;
                            DataSource->Get(ch); // Consume the second quote
                            cell += '"';
                        } else {
                            inQuotes = false;
                        }
                    } else if (!inQuotes && cell.empty()) {
                        inQuotes = true;
                    } else {
                        cell += ch;
                    }
                } else if (ch == Delimiter && !inQuotes) {
                    row.push_back(cell);
                    cell.clear();
                } else {
                    cell += ch;
                }
            }
        
            // Add the last cell if it exists
            if (!cell.empty() || !row.empty() || inQuotes) {
                row.push_back(cell);
            }
        
            return !row.empty();
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
