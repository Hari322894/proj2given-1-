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
                                
                                // Push the current cell and clear it
                                if (nextChar == Delimiter) {
                                    row.push_back(cell);
                                    cell.clear();
                                } else if (nextChar == '\n' || nextChar == '\r') {
                                    row.push_back(cell);
                                    return true;
                                } else if (!DataSource->End()) {
                                    // Unexpected character after closing quote
                                    cell += nextChar;
                                }
                            }
                        }
                    } else {
                        // Starting a quoted section
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
                    return true;    
                } else {
                    // Regular character, add to cell
                    cell += ch;
                }
            }
            
            // If the cell has data and ends with a quote, remove the surrounding quotes
            if (!cell.empty() && inQuotes) {
                // Remove the leading and trailing quotes
                if (cell.front() == '"' && cell.back() == '"') {
                    cell = cell.substr(1, cell.size() - 2);
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
