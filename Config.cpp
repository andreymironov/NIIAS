#include "Config.h"

#include <set>

using namespace Core;
using namespace Math;

namespace Storage
{
    Config::Config(IErrorMessanger& error_messanger, IFileReader& file_reader, IParser& parser, const path_t& file_path) :
        m_error_messanger(error_messanger),
        m_file_reader(file_reader),
        m_parser(parser),
        m_file_path(file_path)
    {
        safe(this, m_error_messanger, &Config::prepare);
    }

    const sensor_positions_t& Config::get_positions(void) const
    {
        return m_sensor_positions;
    }

    void Config::prepare(void)
    {
        const auto config_lines = m_file_reader.read(m_file_path);
        m_config_records = m_parser.read_config(config_lines);
        std::set<sensor_id_t> unique_sensor_ids;
        for (const auto& config_record : m_config_records)
        {
            if (config_record.from_sensor_id == config_record.to_sensor_id)
                m_error_messanger.fire("Same sensor id as from and to in config line.");
            if (config_record.to_sensor_id < config_record.from_sensor_id)
                m_error_messanger.fire("From sensor id is greater than to sensor id.");
            unique_sensor_ids.insert(config_record.from_sensor_id);
            unique_sensor_ids.insert(config_record.to_sensor_id);
        }
        for (auto sensor_id_it = unique_sensor_ids.cbegin(), end_id_it = unique_sensor_ids.cend(); sensor_id_it != end_id_it; ++sensor_id_it)
        {
            if (sensor_id_it == unique_sensor_ids.cbegin())
            {
                m_sensor_positions.insert(std::make_pair(*sensor_id_it, 0.0));
                continue;
            }
            get_position(*sensor_id_it);
        }
    }

    position_t Config::get_position(const sensor_id_t& sensor_id)
    {
        auto found = m_sensor_positions.find(sensor_id);
        if (found != m_sensor_positions.end())
            return found->second;
        for (const auto& config_record : m_config_records)
        {
            if (config_record.to_sensor_id == sensor_id)
            {
                const position_t position = get_position(config_record.from_sensor_id) + config_record.distance;
                m_sensor_positions.insert(std::make_pair(sensor_id, position));
                return position;
            }
        }
        m_error_messanger.fire("Unable to find sensor position due to wrong config line.");
        return 0;
    }
}
