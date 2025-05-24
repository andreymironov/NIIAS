#ifndef IFILEREADER_H
#define IFILEREADER_H

#include "Types.h"

namespace Core
{
    class IFileReader
    {
    public:
        virtual ~IFileReader(void) = default;
        virtual file_lines_t read(const path_t& path) const = 0;

    protected:
        IFileReader(void) = default;

    private:
        IFileReader(const IFileReader&) = delete;
        IFileReader(IFileReader&&) = delete;
        IFileReader& operator=(const IFileReader&) = delete;
        IFileReader& operator=(IFileReader&&) = delete;
    };
}

#endif
