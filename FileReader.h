#ifndef FILEREADER_H
#define FILEREADER_H

#include "IFileReader.h"
#include "IErrorMessanger.h"

namespace Core
{
    class FileReader : public IFileReader
    {
    public:
        explicit FileReader(IErrorMessanger& error_messanger);
        virtual ~FileReader(void) = default;
        virtual file_lines_t read(const path_t& path) const override final;

    private:
        FileReader(const FileReader&) = delete;
        FileReader(FileReader&&) = delete;
        FileReader& operator=(const FileReader&) = delete;
        FileReader& operator=(FileReader&&) = delete;

        IErrorMessanger& m_error_messanger;
    };
}

#endif
