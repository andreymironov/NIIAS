#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <type_traits>
#include <limits>

namespace Core
{
    typedef std::string error_message_t;
    typedef std::wstring path_t;
    typedef std::wstring file_line_t;
    typedef std::vector<file_line_t> file_lines_t;
    typedef std::wstring x_string_t;
    typedef double canpos_t;
    typedef double canrat_t;

    static_assert(1 < sizeof(path_t::value_type), "Path type element is too small");
    static_assert(1 < sizeof(file_line_t::value_type), "File line type element is too small");
    static_assert(1 < sizeof(x_string_t::value_type), "X string type element is too small");
    static_assert(std::is_floating_point_v<canpos_t>, "Canpos type is not floating point type");
    static_assert(std::is_floating_point_v<canrat_t>, "Canrat type is not floating point type");
}

namespace Math
{
    typedef double position_t;
    typedef std::vector<position_t> chasis_formula_t;
    typedef double timestamp_t;
    typedef decltype(position_t{} / timestamp_t{}) velocity_t;

    static_assert(std::is_floating_point_v<position_t>, "Position type is not floating point type");
    static_assert(std::is_floating_point_v<timestamp_t>, "Timestamp type is not floating point type");

    static const timestamp_t s_infinite_time = std::numeric_limits<timestamp_t>::max();

    struct SensorResult;
    struct OverallResult;
}

namespace Storage
{
    typedef uint8_t sensor_id_t;
    typedef std::vector<sensor_id_t> sensor_ids_t;
    typedef std::map<sensor_id_t, Math::position_t> sensor_positions_t;
    typedef uint16_t counter_t;
    typedef std::vector<counter_t> counters_t;
    typedef uint16_t axle_id_t;

    static_assert(std::is_integral_v<sensor_id_t>, "Sensor id type is not integral type");
    static_assert(std::is_unsigned_v<sensor_id_t>, "Sensor id type is not unsigned type");
    static_assert(std::is_integral_v<counter_t>, "Counter type is not integral type");
    static_assert(std::is_unsigned_v<counter_t>, "Counter type is not unsigned type");
    static_assert(std::is_integral_v<axle_id_t>, "Axle id type is not integral type");
    static_assert(std::is_unsigned_v<axle_id_t>, "Axle id type is not unsigned type");

    struct FileRecord;
    struct ConfigRecord;

    static const Core::path_t s_config_filename = L"sensors.conf";
    static const counter_t s_max_counter_value = 65535;
}

#endif
