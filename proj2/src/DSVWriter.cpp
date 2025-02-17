#include "DSVWriter.h"

struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> DataSink;
    char Delimiter;
    bool QuoteAll;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : DataSink(std::move(sink)), Delimiter(delimiter), QuoteAll(quoteall) {}

    bool WriteRow(const std::vector<std::string> &row) {
        for (size_t i = 0; i < row.size(); ++i) {
            std::string cell = row[i];
            bool needsQuotes = QuoteAll || (cell.find(Delimiter) != std::string::npos);

            if (needsQuotes) {
                DataSink->Put('"');
                for (char ch : cell) {
                    if (ch == '"') DataSink->Put('"');
                    DataSink->Put(ch);
                }
                DataSink->Put('"');
            } else {
                for (char ch : cell) DataSink->Put(ch);
            }

            if (i < row.size() - 1) DataSink->Put(Delimiter);
        }
        DataSink->Put('\n');
        return true;
    }
};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(sink, delimiter, quoteall)) {}

CDSVWriter::~CDSVWriter() = default;

bool CDSVWriter::WriteRow(const std::vector<std::string> &row) {
    return DImplementation->WriteRow(row);
}
