#include "DSVWriter.h"
#include "DataSink.h"

struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> Sink;
    char Delimiter;
    bool QuoteAll;
    
    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : Sink(sink), Delimiter(delimiter), QuoteAll(quoteall) {}
    
        bool WriteRow(const std::vector<std::string>& row) {
            for (size_t i = 0; i < row.size(); ++i) {
                bool needsQuotes = QuoteAll || row[i].find(Delimiter) != std::string::npos || row[i].find('"') != std::string::npos;
        
                if (needsQuotes) {
                    Sink->Put('"');
                    for (char ch : row[i]) {
                        if (ch == '"') {
                            Sink->Put('"'); // Escape quotes by doubling them
                            Sink->Put('"');
                        } else {
                            Sink->Put(ch);
                        }
                    }
                    Sink->Put('"');
                } else {
                    for (char ch : row[i]) {
                        Sink->Put(ch);
                    }
                }
        
                if (i < row.size() - 1) {
                    Sink->Put(Delimiter);
                }
            }
            return Sink->Put('\n');
        }
};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {}

CDSVWriter::~CDSVWriter() = default;

bool CDSVWriter::WriteRow(const std::vector<std::string>& row) { return DImplementation->WriteRow(row); }