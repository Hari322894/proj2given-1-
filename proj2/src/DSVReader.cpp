#include "DSVReader.h" //including header file for CDSVReader class usage

//implementing details of CDSV Reader into struct function
struct CDSVReader::SImplementation {
    //shared pointer to datasource in order for reading
    std::shared_ptr<CDataSource> DataSource;
    //char variable used to seperate values in the file
    char Delimiter;

    //i need to intialize my source and delimeter before moving on any further
    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DataSource(std::move(src)), Delimiter(delimiter) {}

    //making this function in order to read the row which is most likely a vector of strings
    bool ReadRow(std::vector<std::string> &row) {
        //start off with a cleared row, therefore use .clear() command in order to
        row.clear();
        //accumulates the cell data we need, which is why its also a string
        std::string cell;
        //character being read from the source
        char ch;
        //check to see if character inside a quoted area
        bool inQuotes = false;
        //check to see if any data was read
        bool dataRead = false;
        // Track if we've seen any non-whitespace characters in the current cell
        bool hasContent = false;
        // Track if we started with a quote
        bool startedWithQuote = false;

        //loop till end of data source
        while (!DataSource->End()) {
            //reads a character and if it cant read the character then we leave and exit
            if (!DataSource->Get(ch)){
                return false;
            }
            //data has been read
            dataRead = true;

            // Handle start of cell
            if (cell.empty() && !hasContent && ch == '"') {
                inQuotes = true;
                startedWithQuote = true;
                hasContent = true;
                continue;
            }

            // Handle quotes
            if (ch == '"') {
                if (inQuotes) {
                    // Check for escaped quotes
                    if (!DataSource->End()) {
                        char nextCh;
                        if (DataSource->Get(nextCh)) {
                            if (nextCh == '"') {
                                cell += '"';
                                continue;
                            } else {
                                // Put back the character we just read
                                DataSource->Unget();
                            }
                        }
                    }
                    inQuotes = false;
                    continue;
                } else if (cell.empty() || !hasContent) {
                    inQuotes = true;
                    continue;
                }
            }

            // Handle delimiters and newlines
            if (!inQuotes && (ch == Delimiter || ch == '\n' || ch == '\r')) {
                // If we started with a quote and ended with a quote, keep the quotes in the output
                if (startedWithQuote) {
                    cell = '"' + cell + '"';
                }
                row.push_back(cell);
                cell.clear();
                hasContent = false;
                startedWithQuote = false;

                if (ch == '\n' || ch == '\r') {
                    // Handle \r\n newline sequence
                    if (ch == '\r' && !DataSource->End()) {
                        char nextCh;
                        if (DataSource->Get(nextCh) && nextCh != '\n') {
                            DataSource->Unget();
                        }
                    }
                    return true;
                }
                continue;
            }

            // Add character to cell
            cell += ch;
            if (!std::isspace(ch)) {
                hasContent = true;
            }
        }

        // Handle last cell if there is one
        if (!cell.empty() || dataRead) {
            if (startedWithQuote) {
                cell = '"' + cell + '"';
            }
            row.push_back(cell);
        }
        return dataRead;
    }
};

//constructor for CDSVReader
CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {}
//destructor for CDSVReader 
CDSVReader::~CDSVReader() = default;
//checking to see if end of data source has been reached
bool CDSVReader::End() const {
    return DImplementation->DataSource->End();
}
//function to read a row of data
bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}