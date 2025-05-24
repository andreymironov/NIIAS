#include "Parser.h"

#include <regex>

using namespace Core;
using namespace Math;

namespace Storage
{
    FileRecord::FileRecord(const sensor_id_t& sensor_id, const velocity_t& velocity, const counter_t& counter, const axle_id_t& axle_id) :
        sensor_id(sensor_id),
        velocity(velocity),
        counter(counter),
        axle_id(axle_id)
    {
    }

    ConfigRecord::ConfigRecord(const sensor_id_t& from_sensor_id, const sensor_id_t& to_sensor_id, const position_t& distance) :
        from_sensor_id(from_sensor_id),
        to_sensor_id(to_sensor_id),
        distance(distance)
    {
    }

    file_records_t Parser::read(const file_lines_t& file_lines) const
    {
        typedef file_line_t::value_type line_char_t;

        static const std::string pattern("^\\[ID=(\\d+); vel=(\\d+,\\d+); time_prev=(\\d+); axle_num=(\\d+)\\]$");

        file_records_t result;
        if (file_lines.empty())
            return result;
        std::basic_string<line_char_t> pattern_(pattern.cbegin(), pattern.cend());
        const std::basic_regex<line_char_t> regex(pattern_.c_str());
        result.reserve(file_lines.size());
        for (const auto& file_line : file_lines)
        {
            std::match_results<std::basic_string<line_char_t>::const_iterator> match;
            if (std::regex_search(file_line, match, regex))
            {
                const sensor_id_t sensor_id = get_value<sensor_id_t>(match[1].str());
                const velocity_t velocity = get_value<velocity_t>(match[2].str(), ',', '.');
                const timestamp_t timestamp = get_value<timestamp_t>(match[3].str());
                const axle_id_t axle_id = get_value<axle_id_t>(match[4].str());
                result.emplace_back(sensor_id, velocity, timestamp, axle_id);
            }
        }
        result.shrink_to_fit();
        return result;
    }

    config_records_t Parser::read_config(const file_lines_t& file_lines) const
    {
        typedef file_line_t::value_type line_char_t;

        static const std::string pattern("^\\[fromID=(\\d+); toID=(\\d+); dist=(\\d+\\.\\d+)]$");

        config_records_t result;
        if (file_lines.empty())
            return result;
        std::basic_string<line_char_t> pattern_(pattern.cbegin(), pattern.cend());
        const std::basic_regex<line_char_t> regex(pattern_.c_str());
        result.reserve(file_lines.size());
        for (const auto& file_line : file_lines)
        {
            std::match_results<std::basic_string<line_char_t>::const_iterator> match;
            if (std::regex_search(file_line, match, regex))
            {
                const sensor_id_t from_sensor_id = get_value<sensor_id_t>(match[1].str());
                const sensor_id_t to_sensor_id = get_value<sensor_id_t>(match[2].str());
                const position_t distance = get_value<position_t>(match[3].str());
                result.emplace_back(from_sensor_id, to_sensor_id, distance);
            }
        }
        result.shrink_to_fit();
        return result;
    }
}
