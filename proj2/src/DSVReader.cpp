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
        
        while (!DataSource->End()) {
            if (!DataSource->Get(ch)) return false;
            hasData = true;
            
            if (ch == '"') {
                // Check if the next character is another quote (escaped quote)
                if (!DataSource->End()) {
                    char nextChar;
                    if (DataSource->Peek(nextChar) && nextChar == '"') {
                        // Consume the peeked character
                        DataSource->Get(nextChar);
                        // Escaped quote, add it
                        cell += '"';
                    } else if (inQuotes) {
                        // End of quoted section
                        inQuotes = false;
                    } else {
                        // Starting a quoted section
                        inQuotes = true;
                    }
                } else if (inQuotes) {
                    // End of quoted section at end of file
                    inQuotes = false;
                } else {
                    // Starting a quoted section at end of file (rare case)
                    inQuotes = true;
                }
            } else if (ch == Delimiter && !inQuotes) {
                // Outside quotes, delimiter means end of cell
                row.push_back(cell);
                cell.clear();
            } else if ((ch == '\n' || ch == '\r') && !inQuotes) {
                // Newline outside quotes means end of row
                if (!cell.empty() || !row.empty()) {
                    row.push_back(cell);
                }
                
                // Skip any following \r\n combination
                if (ch == '\r' && !DataSource->End()) {
                    char nextChar;
                    if (DataSource->Peek(nextChar) && nextChar == '\n') {
                        DataSource->Get(nextChar); // Consume the '\n'
                    }
                }
                
                return true;
            } else {
                // Regular character, add to cell
                cell += ch;
            }
        }
        
        // Add last cell if there was any data
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