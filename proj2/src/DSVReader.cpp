#include "DSVReader.h" //including header file for CDSVReader class usage

//implementing details of DSV Reader into struct function
struct CDSVReader::SImplementation {
    //shared pointer to datasource in order for reading
    std::shared_ptr<CDataSource> DataSource;
    //char variable used to separate values in the file
    char Delimiter;

    //initialize my source and delimiter before moving on any further
    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DataSource(std::move(src)), Delimiter(delimiter) {}

    //reading the row which is most likely a vector of strings
    bool ReadRow(std::vector<std::string> &row) {
        //start off with a cleared row
        row.clear();
        
        //string to store the current cell being processed
        std::string cell;
        //character being read from the source
        char ch;
        //check if we're inside a quoted section
        bool inQuotes = false;
        //track if we've read any data at all
        bool hasData = false;

        //keep reading until we hit the end of the data source
        while (!DataSource->End()) {
            //if we can't read a character, return false for error
            if (!DataSource->Get(ch)) return false;
            //mark that we've read some data
            hasData = true;

            //handle quote character processing
            if (ch == '"') {
                //need to check for escaped quotes (two quotes in a row)
                if (!DataSource->End()) {
                    //look at next character without consuming it
                    char nextChar;
                    if (DataSource->Peek(nextChar) && nextChar == '"') {
                        //found escaped quote, consume the second quote
                        DataSource->Get(nextChar);
                        //add single quote to cell
                        cell += '"';
                    } 
                    //check if we're ending a quoted section
                    else if (inQuotes) {
                        //ending quoted section
                        inQuotes = false;
                    }
                    //starting new quoted section
                    else {
                        inQuotes = true;
                    }
                }
                //handling quotes at end of file
                else if (inQuotes) {
                    //end quote section at file end
                    inQuotes = false;
                }
                else {
                    //start quote section at file end
                    inQuotes = true;
                }
            }
            //found delimiter outside quotes means end of current cell
            else if (ch == Delimiter && !inQuotes) {
                //add completed cell to row
                row.push_back(cell);
                //clear cell for next value
                cell.clear();
            }
            //handle end of row with newlines
            else if ((ch == '\n' || ch == '\r') && !inQuotes) {
                //add final cell if we have data
                if (!cell.empty() || !row.empty()) {
                    row.push_back(cell);
                }

                //handle windows-style line endings (\r\n)
                if (ch == '\r' && !DataSource->End()) {
                    //peek at next character
                    char nextChar;
                    if (DataSource->Peek(nextChar) && nextChar == '\n') {
                        //consume the \n if found
                        DataSource->Get(nextChar);
                    }
                }

                //return true for successful row read
                return true;
            }
            //regular character, add to current cell
            else {
                cell += ch;
            }
        }

        //handle any remaining data in last cell
        if (!cell.empty() || hasData) {
            row.push_back(cell);
        }

        //return whether we found any data
        return hasData;
    }
};

//constructor for DSV Reader class
CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(src, delimiter)) {}

//destructor for DSV Reader class
CDSVReader::~CDSVReader() = default;

//check if we've reached end of data source
bool CDSVReader::End() const {
    return DImplementation->DataSource->End();
}

//read a row of data from the source
bool CDSVReader::ReadRow(std::vector<std::string> &row) {
    return DImplementation->ReadRow(row);
}