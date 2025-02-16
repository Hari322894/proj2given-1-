#include "DSVWriter.h"
#include <sstream>

struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> DDataSink;
    char DDelimiter;
    bool DQuoteAll;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : DDataSink(sink), DDelimiter(delimiter), DQuoteAll(quoteall) {}

    bool WriteRow(const std::vector<std::string> &row) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) {
                DDataSink->Write(std::vector<char>{DDelimiter});
            }
            std::string cell = row[i];
            bool needsQuotes = DQuoteAll || cell.find(DDelimiter) != std::string::npos || cell.find('"') != std::string::npos || cell.find('\n') != std::string::npos;
            if (needsQuotes) {
                DDataSink->Write(std::vector<char>{'"'});
                for (char ch : cell) {
                    if (ch == '"') {
                        DDataSink->Write(std::vector<char>{'"', '"'});
                    } else {
                        DDataSink->Write(std::vector<char>{ch});
                    }
                }
                DDataSink->Write(std::vector<char>{'"'});
            } else {
                DDataSink->Write(std::vector<char>(cell.begin(), cell.end()));
            }
        }
        DDataSink->Write(std::vector<char>{'\n'});
        return true;
    }
};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {}

CDSVWriter::~CDSVWriter() = default;

bool CDSVWriter::WriteRow(const std::vector<std::string> &row) {
    return DImplementation->WriteRow(row);
}