#ifndef IPARSER_H
#define IPARSER_H

#include "Types.h"

namespace Storage
{
    class IParser
    {
    public:
        virtual ~IParser(void) = default;
        virtual std::vector<FileRecord> read(const Core::file_lines_t& file_lines) const = 0;
        virtual std::vector<ConfigRecord> read_config(const Core::file_lines_t& file_lines) const = 0;

    protected:
        IParser(void) = default;

    private:
        IParser(const IParser&) = delete;
        IParser(IParser&&) = delete;
        IParser& operator=(const IParser&) = delete;
        IParser& operator=(IParser&&) = delete;
    };
}

#endif
