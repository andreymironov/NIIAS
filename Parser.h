#ifndef PARSER_H
#define PARSER_H

#include "IParser.h"

namespace Storage
{
    struct FileRecord
    {
        FileRecord(const sensor_id_t& sensor_id, const Math::velocity_t& velocity, const counter_t& counter, const axle_id_t& axle_id);

        const sensor_id_t sensor_id;
        const Math::velocity_t velocity;
        const counter_t counter;
        const axle_id_t axle_id;
    };

    typedef std::vector<FileRecord> file_records_t;

    struct ConfigRecord
    {
        ConfigRecord(const sensor_id_t& from_sensor_id, const sensor_id_t& to_sensor_id, const Math::position_t& distance);

        const sensor_id_t from_sensor_id;
        const sensor_id_t to_sensor_id;
        const Math::position_t distance;
    };

    typedef std::vector<ConfigRecord> config_records_t;

    class Parser : public IParser
    {
    public:
        explicit Parser(void) = default;
        virtual ~Parser(void) = default;
        virtual file_records_t read(const Core::file_lines_t& file_lines) const override final;
        virtual config_records_t read_config(const Core::file_lines_t& file_lines) const override final;

    private:
        Parser(const Parser&) = delete;
        Parser(Parser&&) = delete;
        Parser& operator=(const Parser&) = delete;
        Parser& operator=(Parser&&) = delete;

        template <typename T, typename C>
        T get_value(const std::basic_string<C>& string, const char from = ' ', const char to = ' ') const
        {
            std::string value(string.cbegin(), string.cend());
            if (from != to)
            {
                auto found = value.find(from);
                if (found != value.npos)
                    value[found] = to;
            }
            if constexpr (std::is_floating_point_v<T>)
                return static_cast<T>(std::stold(value.c_str()));
            else
                return static_cast<T>(std::stoull(value.c_str()));
        }
    };
}

#endif
