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
                    if (inQuotes) {
                        // Check if the next character is another quote (escaped quote)
                        if (!DataSource->End()) {
                            char nextChar;
                            if (DataSource->Get(nextChar) && nextChar == '"') {
                                // Escaped quote, add it
                                cell += '"';
                            } else {
                                // End of quoted section
                                inQuotes = false;
                                if (nextChar == Delimiter || nextChar == '\n' || nextChar == '\r') {
                                    // Valid termination, process as normal
                                    if (nextChar == Delimiter) {
                                        row.push_back(cell);
                                        cell.clear();
                                    } else {
                                        row.push_back(cell);
                                        return true;
                                    }
                                } else {
                                    // If the next character is unexpected, include it
                                    cell += nextChar;
                                }
                            }
                        }
                    } else {
                        // Starting a quoted section
                        inQuotes = true;
                        cell += '"'; // Add the opening quote to the cell
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
