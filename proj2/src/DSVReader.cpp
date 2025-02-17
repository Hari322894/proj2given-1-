#include "DSVReader.h"
#include <sstream>

struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> DataSource;
    char Delimiter;
    bool ReadStarted = false;  // Flag to indicate if reading has started

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DataSource(std::move(src)), Delimiter(delimiter) {}

    bool ReadRow(std::vector<std::string> &row) {
        row.clear();
        std::string cell;
        char ch;
        bool inQuotes = false;

        // Mark that reading has started
        ReadStarted = true;

        while (!DataSource->End()) {
            if (!DataSource->Get(ch)) return false;

            if (ch == '"') {
                // Handle embedded quotes ("")
                if (inQuotes && !DataSource->End()) {
                    char nextChar;
                    if (DataSource->Get(nextChar)) {
                        if (nextChar == '"') {
                            // Add escaped quote to the cell
                            cell += '"';
                        } else {
                            // Not an escaped quote, push back the character
                            DataSource->Unget(nextChar);
                            inQuotes = !inQuotes;  // Toggle quote state
                        }
                    }
                } else {
                    // Toggle quote state
                    inQuotes = !inQuotes;
                }
            } else if (ch == Delimiter && !inQuotes) {
                // If delimiter is encountered outside quotes, add the cell to the row
                row.push_back(cell);
                cell.clear();
            } else {
                // Append the character to the current cell
                cell += ch;
            }
        }

        // Add the last cell if there's content
        if (!cell.empty() || !row.empty()) {
            row.push_back(cell);
        }

        return true;
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
