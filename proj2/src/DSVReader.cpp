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
// State to track if the previous character was a quote
        bool pendingQuote = false; 

 //loop till end of data source
        while (!DataSource->End()) {
 //reads a character and if it cant read the character then we leave and exit
            if (!DataSource->Get(ch)){
                return false;
            } 
 //data has been read
            dataRead = true;
 //creating an if statement to handle the quotes
 if (ch == '"') {
    if (inQuotes) {
        // if there was a pending quote from the previous iteration then we treat it as an escaped quote
        if (pendingQuote) {
            cell += '"'; // add escaped quote to the cell
            pendingQuote = false; // reset
        } else {
            pendingQuote = true; // use this quote as pending for the next iteration
        }
    } else {
// starting a quoted section
        inQuotes = true;
        pendingQuote = false; // clear any previous pending quote
    }
} else {
    if (pendingQuote) {
        // if the previous character was a quote and no second quote followed, end the quoted section
        inQuotes = false;
        pendingQuote = false;
    }

    if (ch == Delimiter && !inQuotes) {
        // Delimiter marks the end of the cell
        row.push_back(cell);//add completed cell to row
        cell.clear();//clear for next value
    } else if ((ch == '\n' || ch == '\r') && !inQuotes) {
        // end of row
        if (!cell.empty() || !row.empty()) {
            row.push_back(cell);
        }
        // handles a potential "\r\n" newline sequence
        if (ch == '\r' && !DataSource->End()) {
        // check if the current character is a \r
       // and see that we are not at the end of the data source
            char nextChar;
            if (DataSource->Get(nextChar) && nextChar == '\n') {
        // If the next character is a \n, this indicates
        // \r\n. Consume the '\n' to avoid
        // treating it as the start of a new row.
            }
        }
        return true;
        //end of row has been reached when returning true
    } else {
        // add regular character to the current cell
        cell += ch;
    }
        }
        
        // add last cell if there was any data
        if (!cell.empty() || dataRead) {
            row.push_back(cell);
        }
        //if any data was read then it would return true here at the end as I let it be true earlier
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