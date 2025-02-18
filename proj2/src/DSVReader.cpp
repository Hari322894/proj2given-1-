#include "DSVReader.h" // including header file for CDSVReader class usage

// Implementing details of DSV Reader into struct function
struct CDSVReader::SImplementation {
    // Shared pointer to datasource in order for reading
    std::shared_ptr<CDataSource> DataSource;
    // Char variable used to separate values in the file
    char Delimiter;

    // Initialize my source and delimiter before moving on any further
    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DataSource(std::move(src)), Delimiter(delimiter) {}

    // Reading the row which is most likely a vector of strings
    bool ReadRow(std::vector<std::string>& currentRow) {
        // Begin with an empty row
        currentRow.clear();
        
        // A string to get data for each cell
        std::string currentCell;
        // A variable to store the character read from the data source
        char currentChar;
        // Determines if we are inside a quoted string
        bool isInQuotes = false;
        // To track whether any data has been read
        bool data = false;
    
        // Read characters until reaching the end of the data source
        while (!DataSource->End()) {
            // If unable to read a character, return false
            if (!DataSource->Get(currentChar)) {
                return false;
            }
    
            // If a character was successfully read, then we've encountered data
            data = true;
    
            // Having quotes for the current characters
            if (currentChar == '"') {
                // Check for double quotes in a row
                if (!DataSource->End()) {
                    char nextChar;
                    // Attempt to peek at the next character from the DataSource.
                    bool peekResult = DataSource->Peek(nextChar);
                    if (peekResult && nextChar == '"') {
                        // If the next character is another quote, treat it as an escaped quote
                        DataSource->Get(nextChar); // Takes the second quote
                        currentCell += '"'; // Add a single quote to the current cell
                    } else if (isInQuotes) {
                        // We are inside quotes and find another quote, it’s the end of the quoted section
                        isInQuotes = false;
                    } else {
                        // We are starting a new quoted section
                        isInQuotes = true;
                    }
                } else if (isInQuotes) {
                    // If we are at the end of the file and inside quotes, close quote section
                    isInQuotes = false;
                } else {
                    // If at the end of the file and not inside quotes, begin new quote section
                    isInQuotes = true;
                }
            }
            // If we hit a delimiter and we're not inside quotes, it marks the end of the current cell
            else if (currentChar == Delimiter && !isInQuotes) {
                currentRow.push_back(currentCell); // Add the completed cell to the row
                currentCell.clear(); // Prepare for the next cell by clearing the cell
            }
            // End of the row detected (\n or \r return), unless inside quotes
            else if ((currentChar == '\n' || currentChar == '\r') && !isInQuotes) {
                if (!currentCell.empty() || !currentRow.empty()) {
                    currentRow.push_back(currentCell); // Add any remaining data in the cell with this line
                }
    
                // \r\n handling
                if (currentChar == '\r' && !DataSource->End()) {
                    char nextChar;
                    bool peekResult = DataSource->Peek(nextChar);
                    if (peekResult && nextChar == '\n') {
                        // If the next character is '\n', take it in to avoid treating it as part of the next row
                        DataSource->Get(nextChar);
                    }
                }
    
                return true; // Successfully read the row
            }
            // Add regular character to the current cell
            else {
                currentCell += currentChar;
            }
        }
    
        // Any remaining data in the current cell, push it into the row
        if (!currentCell.empty() || data) {
            currentRow.push_back(currentCell);
        }
    
        // Return true if any content was read, otherwise false
        return data;
    }
};

// Constructor for DSV Reader class
CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {}

// Destructor for DSV Reader class
CDSVReader::~CDSVReader() = default;

// Check if we've reached the end of data source
bool CDSVReader::End() const {
    return DImplementation->DataSource->End();
}

// Read a row of data from the source
bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}
