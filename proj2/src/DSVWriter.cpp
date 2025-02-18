#include "DSVWriter.h"
#include "DataSink.h"

// implementing details of DSV Writer into struct function
struct CDSVWriter::SImplementation {
    // shared pointer to the data sink for writing the output later on in the code
    std::shared_ptr<CDataSink> Sink;
    // character used to separate values in the output called delimiter
    char Delimiter;
    // determine if all values should be quoted, regardless of content
    bool QuoteAll;

    // initialize the data sink, delimiter, and quote-all option
    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : Sink(sink), Delimiter(delimiter), QuoteAll(quoteall) {}
    
    // writes a row of data to the sink, ensuring proper DSV formatting
    bool WriteRow(const std::vector<std::string>& row) {
        // iterate through each cell in the row
        for (size_t i = 0; i < row.size(); ++i) {
            // determine if the cell needs to be enclosed in quotes
            // a cell needs quotes if:
            // QuoteAll is true, the cell contains the delimiter, the cell contains quotes
            bool needsQuotes = QuoteAll || row[i].find(Delimiter) != std::string::npos || row[i].find('"') != std::string::npos;
           //create an if statement here
            if (needsQuotes) {
                // start the quoted cell by writing an opening quote
                Sink->Put('"');
                // write each character of the cell with a for loop
                for (char ch : row[i]) {
                    if (ch == '"') {
                        // escape quotes by writing two quotes
                        Sink->Put('"');
                        Sink->Put('"');
                    } else {
                        // write the regular character
                        Sink->Put(ch);
                    }
                }
                // close the quoted cell with a quote
                Sink->Put('"');
            } else {
                // If the cell doesn't need quotes, write it directly
                //use a for loop to iterate through the characters
                for (char ch : row[i]) {
                    Sink->Put(ch);
                }
            }

            // if this is not the last cell, write the delimiter to separate the cells
            if (i < row.size() - 1) {
                Sink->Put(Delimiter);
            }
        }
        // write a newline to indicate the end of the row
        return Sink->Put('\n');
    }
};

// Constructor for the CDSVWriter class
CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {}

// Destructor for the CDSVWriter class
CDSVWriter::~CDSVWriter() = default;

// write a row of data called here
bool CDSVWriter::WriteRow(const std::vector<std::string>& row) { 
    return DImplementation->WriteRow(row);
}
