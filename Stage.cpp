#include "Stage.h"
#include "Parser.h"

#include <sstream>
#include <iomanip>

using namespace Core;
using namespace Math;
using namespace Storage;

namespace Model
{
    Stage::Stage(IWindow& window, IErrorMessanger& error_messanger, IFileReader& file_reader, IParser& parser, const x_string_t& file_path, const IConfig& config) :
        m_window(window),
        m_error_messanger(error_messanger),
        m_file_reader(file_reader),
        m_parser(parser),
        m_file_path(file_path),
        m_config(config),
        m_model(m_error_messanger, config),
        m_selected_sensor_index(0),
        m_is_time_relative(true)
    {
        safe(this, m_error_messanger, &Stage::prepare);
    }

    Stage::~Stage(void)
    {
        m_window.clear_overall_info();
        m_window.clear_sensor_info();
        m_window.clear_sensor_names();
        m_window.clear_sensor_lines();
        m_window.clear_error_lines();
    }

    void Stage::on_sensor_selection(const size_t index)
    {
        safe(this, m_error_messanger, &Stage::on_sensor_selection_helper, index);
    }

    void Stage::on_time_selection(const bool is_relative)
    {
        safe(this, m_error_messanger, &Stage::on_time_selection_helper, is_relative);
    }

    const OverallResult& Stage::get_overall_result(void) const
    {
        return m_model.get_overall_result();
    }

    timestamp_t Stage::get_max_absolute_timestamp(void) const
    {
        return m_model.get_max_absolute_timestamp();
    }

    std::pair<position_t, velocity_t> Stage::get_info_at_time(const timestamp_t& timestamp) const
    {
        return m_model.get_info_at_time(timestamp);
    }

    Storage::counters_t Stage::get_counters_at_time(const Math::timestamp_t& timestamp) const
    {
        return m_model.get_counters_at_time(timestamp);
    }

    void Stage::prepare(void)
    {
        const auto file_lines = m_file_reader.read(m_file_path);
        const auto file_records = m_parser.read(file_lines);
        for (const auto& file_record : file_records)
        {
            std::wstringstream stream;
            stream << std::setprecision(3);
            stream << L"Sensor #" << file_record.sensor_id << L": Axle #" << file_record.axle_id << L", Vel " << file_record.velocity << L" m/s, Counter " << file_record.counter << " ms";
            m_window.add_sensor_line(stream.str());
        }
        for (const auto& file_record : file_records)
        {
            const timestamp_t relative_timestamp = (file_record.counter == s_max_counter_value ? s_infinite_time : static_cast<timestamp_t>(file_record.counter) / static_cast<timestamp_t>(1000));
            m_model.add_sensor_data(file_record.sensor_id, file_record.axle_id, file_record.velocity, relative_timestamp);
        }
        const auto& sensor_ids = m_model.get_sensor_ids();
        for (const auto& sensor_id : sensor_ids)
        {
            std::wstringstream stream;
            stream << L"Sensor #" << sensor_id;
            m_window.add_sensor_name(stream.str());
        }
        m_model.process();
        const auto& overall_result = m_model.get_overall_result();
        {
            std::wstringstream stream;
            stream << L"Chasis formula:";
            for (const auto& distance : overall_result.chasis_formula)
                stream << L" " << static_cast<int>(distance * 1000.0);
            stream << L" mm";
            m_window.set_overall_chasis_formula(stream.str());
        }
        {
            std::wstringstream stream;
            stream << L"Length formula:";
            for (const auto& distance : overall_result.length_formula)
                stream << L" " << static_cast<int>(distance * 1000.0);
            stream << L" mm";
            m_window.set_overall_length_formula(stream.str());
        }
    }

    void Stage::on_sensor_selection_helper(const size_t index)
    {
        m_window.clear_sensor_info();
        const auto& sensor_ids = m_model.get_sensor_ids();
        const auto sensor_id = sensor_ids[index];
        const auto& sensor_result = m_model.get_sensor_result(sensor_id);
        {
            std::wstringstream stream;
            stream << L"Sensor #" << sensor_id << L" info (" << (m_is_time_relative ? L"relative" : L"absolute") << L" time):";
            m_window.set_sensor_title(stream.str());
        }
        {
            std::wstringstream stream;
            stream << L"Chasis formula:";
            for (const auto& distance : sensor_result.chasis_formula)
                stream << L" " << static_cast<int>(distance * 1000.0);
            stream << L" mm";
            m_window.set_sensor_chasis_formula(stream.str());
        }
        for (const auto& item : sensor_result.items)
        {
            std::wstringstream stream;
            stream << std::setprecision(3);
            stream << (m_is_time_relative ? *item.relative_timestamp : *item.absolute_timestamp) << L" s: Axle #" << *item.axle_id << L" @ " << *item.velocity << L" m/s, Train length " << *item.relative_passed_distance << " m";
            m_window.add_sensor_info_line(stream.str());
        }
        {
            std::wstringstream stream;
            stream << std::setprecision(3);
            stream << L"Average velocity: " << sensor_result.average_velocity << L" m/s";
            m_window.set_sensor_average_velocity(stream.str());
        }
        {
            std::wstringstream stream;
            stream << std::setprecision(3);
            stream << L"Velocity deviation: " << sensor_result.velocity_deviation << L" m/s";
            m_window.set_sensor_velocity_deviation(stream.str());
        }
        m_selected_sensor_index = index;
    }

    void Stage::on_time_selection_helper(const bool is_relative)
    {
        if (m_is_time_relative == is_relative)
            return;
        m_is_time_relative = is_relative;
        on_sensor_selection_helper(m_selected_sensor_index);
    }
}
