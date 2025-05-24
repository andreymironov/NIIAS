#include "Model.h"

#include <cmath>

using namespace Core;
using namespace Math;
using namespace Storage;

namespace Math
{
    SensorResult::SensorResult(void) :
        average_velocity(0),
        velocity_deviation(0)
    {
    }

    SensorResult::Item::Item(const axle_id_t* const axle_id, const velocity_t* const velocity, const timestamp_t* const relative_timestamp, const timestamp_t* const absolute_timestamp, const position_t* const relative_passed_distance) :
        axle_id(axle_id),
        velocity(velocity),
        relative_timestamp(relative_timestamp),
        absolute_timestamp(absolute_timestamp),
        relative_passed_distance(relative_passed_distance)
    {
    }

    Model::Model(IErrorMessanger& error_messanger, const IConfig& config) :
        m_error_messanger(error_messanger),
        m_config(config),
        m_max_absolute_timestamp(0.0)
    {
    }

    void Model::add_sensor_data(const sensor_id_t& sensor_id, const axle_id_t& axle_id, const velocity_t& velocity, const timestamp_t& timespan)
    {
        if (velocity < 0)
            m_error_messanger.fire("Negative velocity.");
        if (timespan < 0)
            m_error_messanger.fire("Negative timespan.");
        auto& sensor = get_sensor(sensor_id);
        auto& new_data = sensor.add_data(axle_id, velocity, timespan);
        m_sequence.push_back(std::make_pair(sensor_id, &new_data));
    }

    const sensor_ids_t& Model::get_sensor_ids(void) const
    {
        return m_sensor_ids;
    }

    void Model::process(void)
    {
        for (auto& sensor : m_sensors)
            sensor.second.process();
        size_t chasis_formula_size = 0;
        for (auto& sensor : m_sensors)
        {
            sensor.second.prepare_result();
            const auto& sensor_result = sensor.second.get_result();
            if (chasis_formula_size == 0)
                chasis_formula_size = sensor_result.chasis_formula.size();
            else if (chasis_formula_size != sensor_result.chasis_formula.size())
                m_error_messanger.fire("Different chasis formula length.");
        }
        auto& chasis_formula = m_overall_result.chasis_formula;
        chasis_formula.resize(chasis_formula_size, 0);
        const position_t weight = 1.0 / static_cast<position_t>(m_sensors.size());
        for (const auto& sensor : m_sensors)
        {
            const auto& sensor_result = sensor.second.get_result();
            const auto& sensor_chasis_formula = sensor_result.chasis_formula;
            for (size_t index = 0; index < chasis_formula_size; ++index)
                chasis_formula[index] += weight * sensor_chasis_formula[index];
        }
        auto& length_formula = m_overall_result.length_formula;
        length_formula.resize(chasis_formula_size + 1, 0);
        for (size_t index = 1; index < chasis_formula_size + 1; ++index)
            length_formula[index] = chasis_formula[index - 1] + length_formula[index - 1];
        const auto& sensor_positions = m_config.get_positions();
        const auto first_sensor_id = sensor_positions.cbegin()->first;
        m_sensors.find(first_sensor_id)->second.set_delay(0.0);
        for (auto item_it = m_sequence.begin(), end_it = m_sequence.end(); item_it != end_it; ++item_it)
        {
            const auto sensor_id = item_it->first;
            const auto axle_id = item_it->second->axle_id;
            const auto sensor_position = sensor_positions.find(sensor_id)->second;
            auto& sensor_data = *(item_it->second);
            const auto axle_to_head_distance = length_formula[axle_id];
            sensor_data.absolute_head_position = sensor_position + axle_to_head_distance;
            auto found = m_sensors.find(sensor_id);
            if (found == m_sensors.end())
                m_error_messanger.fire("Sensor ID mismatch.");
            if (0.0 <= found->second.get_delay())
            {
                const auto time_delay = found->second.get_delay();
                sensor_data.absolute_timestamp = sensor_data.relative_timestamp + time_delay;
                m_max_absolute_timestamp = std::max(m_max_absolute_timestamp, sensor_data.absolute_timestamp);
                continue;
            }
            if (item_it == m_sequence.begin())
                m_error_messanger.fire("Sensor id config precedence mismatch.");
            auto prev_item_it = std::prev(item_it);
            const auto prev_sensor_id = prev_item_it->first;
            if (prev_sensor_id == sensor_id)
                m_error_messanger.fire("Sensor id precedence mismatch.");
            const auto& prev_sensor_data = *(prev_item_it->second);
            const auto prev_sensor_position = sensor_positions.find(prev_sensor_id)->second;
            const auto delta_position = sensor_position - prev_sensor_position;
            const auto prev_axle_to_head_distance = length_formula[prev_sensor_data.axle_id];
            const auto head_to_sensor_distance = std::max(0.0, delta_position - prev_axle_to_head_distance);
            const auto average_velocity = (prev_sensor_data.velocity + sensor_data.velocity) / 2.0;
            const auto travel_time = head_to_sensor_distance / average_velocity;
            const auto absolute_timestamp = prev_sensor_data.absolute_timestamp + travel_time;
            const auto time_delay = absolute_timestamp - sensor_data.relative_timestamp;
            if (time_delay <= 0.0)
                m_error_messanger.fire("Sensor data precedence mismatch.");
            sensor_data.absolute_timestamp = sensor_data.relative_timestamp + time_delay;
            found->second.set_delay(time_delay);
            m_max_absolute_timestamp = std::max(m_max_absolute_timestamp, sensor_data.absolute_timestamp);
        }
    }

    const SensorResult& Model::get_sensor_result(const sensor_id_t& sensor_id) const
    {
        const auto& sensor = get_sensor(sensor_id);
        return sensor.get_result();
    }

    const OverallResult& Model::get_overall_result(void) const
    {
        return m_overall_result;
    }

    timestamp_t Model::get_max_absolute_timestamp(void) const
    {
        return m_max_absolute_timestamp;
    }

    std::pair<position_t, velocity_t> Model::get_info_at_time(const timestamp_t& timestamp) const
    {
        if (timestamp <= 0)
            return std::make_pair(m_sequence.front().second->absolute_head_position, m_sequence.front().second->velocity);
        for (auto item_it = m_sequence.cbegin(), next_it = std::next(item_it), end_it = m_sequence.cend(); next_it != end_it; ++item_it, ++next_it)
        {
            if ((item_it->second->absolute_timestamp <= timestamp) && (timestamp < next_it->second->absolute_timestamp))
            {
                const auto absolute_head_position = lerp(item_it->second->absolute_timestamp, next_it->second->absolute_timestamp, item_it->second->absolute_head_position, next_it->second->absolute_head_position, timestamp);
                const auto velocity = lerp(item_it->second->absolute_timestamp, next_it->second->absolute_timestamp, item_it->second->velocity, next_it->second->velocity, timestamp);
                return std::make_pair(absolute_head_position, velocity);
            }
        }
        return std::make_pair(m_sequence.back().second->absolute_head_position, m_sequence.back().second->velocity);
    }

    counters_t Model::get_counters_at_time(const timestamp_t& timestamp) const
    {
        static const timestamp_t max_timestamp = static_cast<timestamp_t>(s_max_counter_value) / 1000.0;
        counters_t result(m_sensors.size(), s_max_counter_value);
        if (timestamp <= 0)
            return result;
        size_t index = 0;
        for (const auto& sensor : m_sensors)
        {
            const auto& sensor_result = sensor.second.get_result();
            const auto& sensor_items = sensor_result.items;
            for (auto item_it = sensor_items.crbegin(), end_it = sensor_items.crend(); item_it != end_it; ++item_it)
            {
                const auto delta_time = timestamp - *item_it->absolute_timestamp;
                if (0.0 <= delta_time)
                {
                    result[index] = static_cast<counter_t>(std::ceil(1000.0 * std::min(max_timestamp, delta_time)));
                    break;
                }
            }
            ++index;
        }
        return result;
    }

    Model::SensorData::SensorData(const axle_id_t& axle_id, const velocity_t& velocity, const timestamp_t& relative_timestamp) :
        axle_id(axle_id),
        velocity(velocity),
        relative_timestamp(relative_timestamp),
        absolute_timestamp(0),
        relative_local_passed_distance(0),
        relative_passed_distance(0),
        absolute_head_position(0)
    {
    }

    Model::Sensor::Sensor(IErrorMessanger& error_messanger) :
        m_error_messanger(error_messanger),
        m_delay(-1.0)
    {
    }

    Model::Sensor::Sensor(Sensor&& other) :
        m_error_messanger(other.m_error_messanger),
        m_delay(other.m_delay)
    {
        std::swap(m_data, other.m_data);
    }

    Model::SensorData& Model::Sensor::add_data(const axle_id_t& axle_id, const velocity_t& velocity, const timestamp_t& timespan)
    {
        if (!m_data.empty())
        {
            const auto& last_data = m_data.back();
            if (axle_id != last_data.axle_id + 1)
                m_error_messanger.fire("New axle id is not successor of previous.");
            const timestamp_t relative_timestamp = last_data.relative_timestamp + timespan;
            m_data.emplace_back(axle_id, velocity, relative_timestamp);
            return m_data.back();
        }
        if (timespan != s_infinite_time)
            m_error_messanger.fire("Initial timespan is not infinite.");
        m_data.emplace_back(axle_id, velocity, 0);
        return m_data.back();
    }

    void Model::Sensor::process(void)
    {
        if (m_data.size() < 2)
            m_error_messanger.fire("Chasis formula inference requires at least two axis.");
        position_t relative_passed_distance = 0;
        for (auto data_it = std::next(m_data.begin()), end_it = m_data.end(); data_it != end_it; ++data_it)
        {
            auto& data = *data_it;
            auto& prev_data = *(std::prev(data_it));
            const auto delta_time = data.relative_timestamp - prev_data.relative_timestamp;
            const auto average_velocity = (data.velocity + prev_data.velocity) / 2.0;
            const auto relative_local_passed_distance = average_velocity * delta_time;
            relative_passed_distance += relative_local_passed_distance;
            data.relative_local_passed_distance = relative_local_passed_distance;
            data.relative_passed_distance = relative_passed_distance;
        }
        velocity_t average_velocity = 0;
        for (const auto& data : m_data)
            average_velocity += data.velocity;
        average_velocity /= static_cast<velocity_t>(m_data.size());
        m_result.average_velocity = average_velocity;
        velocity_t velocity_deviation = 0;
        for (const auto& data : m_data)
            velocity_deviation += std::abs(data.velocity - average_velocity);
        velocity_deviation /= static_cast<velocity_t>(m_data.size());
        m_result.velocity_deviation = velocity_deviation;
    }

    void Model::Sensor::prepare_result(void)
    {
        auto& chasis_formula = m_result.chasis_formula;
        auto& items = m_result.items;
        for (auto data_it = m_data.begin(), end = m_data.end(); data_it != end; ++data_it)
        {
            if (data_it != m_data.begin())
                chasis_formula.push_back(data_it->relative_local_passed_distance);
            items.emplace_back(&data_it->axle_id, &data_it->velocity, &data_it->relative_timestamp, &data_it->absolute_timestamp, &data_it->relative_passed_distance);
        }
    }

    const SensorResult& Model::Sensor::get_result(void) const
    {
        return m_result;
    }

    timestamp_t Model::Sensor::get_delay(void) const
    {
        return m_delay;
    }

    void Model::Sensor::set_delay(const timestamp_t& delay)
    {
        m_delay = delay;
    }

    const Model::Sensor& Model::get_sensor(const sensor_id_t& sensor_id) const
    {
        auto found = m_sensors.find(sensor_id);
        if (found == m_sensors.cend())
            m_error_messanger.fire("Sensor not found.");
        return found->second;
    }

    Model::Sensor& Model::get_sensor(const sensor_id_t& sensor_id)
    {
        auto found = m_sensors.find(sensor_id);
        if (found != m_sensors.end())
            return found->second;
        auto inserted = m_sensors.insert(std::make_pair(sensor_id, Sensor(m_error_messanger)));
        if (!inserted.second)
            m_error_messanger.fire("Can't add new sensor.");
        m_sensor_ids.push_back(sensor_id);
        return inserted.first->second;
    }
}
