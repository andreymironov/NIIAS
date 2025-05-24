#include "FileReader.h"

#include <fstream>
#include <locale>
#include <codecvt>

namespace Core
{
    FileReader::FileReader(IErrorMessanger& error_messanger) :
        m_error_messanger(error_messanger)
    {
    }

    file_lines_t FileReader::read(const path_t& path) const
    {
        typedef file_line_t::value_type line_char_t;

        std::wifstream file(std::string(path.cbegin(), path.cend()));
        if (!file.is_open())
            m_error_messanger.fire("Can't read file");
        const std::locale conversion_locale(std::locale(), new std::codecvt_utf8<line_char_t>());
        file.imbue(conversion_locale);
        std::basic_string<line_char_t> line;
        file_lines_t file_lines;
        while (std::getline(file, line))
        {
            auto found = line.find(L'\r');
            if (found != line.npos)
                line.erase(found, 1);
            if (!line.empty())
                file_lines.push_back(line);
        }
        file.close();
        return file_lines;
    }
}
