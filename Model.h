#ifndef MODEL_H
#define MODEL_H

#include "IErrorMessanger.h"
#include "IModel.h"
#include "IConfig.h"

#include <vector>
#include <list>
#include <map>

namespace Math
{
    template <typename X, typename Y>
    Y lerp(const X& x_begin, const X& x_end, const Y& y_begin, const Y& y_end, const X& x)
    {
        if (x_end == x_begin)
            return (y_end + y_begin) / static_cast<Y>(2);
        return y_begin + (y_end - y_begin) * static_cast<Y>((x - x_begin) / (x_end - x_begin));
    }

    struct SensorResult
    {
        explicit SensorResult(void);

        struct Item
        {
            Item(const Storage::axle_id_t* const axle_id, const velocity_t* const velocity, const timestamp_t* const relative_timestamp, const timestamp_t* const absolute_timestamp, const position_t* const relative_passed_distance);

            const Storage::axle_id_t* const axle_id;
            const velocity_t* const velocity;
            const timestamp_t* const relative_timestamp;
            const timestamp_t* const absolute_timestamp;
            const position_t* const relative_passed_distance;
        };
        typedef std::vector<Item> items_t;

        chasis_formula_t chasis_formula;
        items_t items;
        velocity_t average_velocity;
        velocity_t velocity_deviation;
    };

    struct OverallResult
    {
        chasis_formula_t chasis_formula;
        chasis_formula_t length_formula;
    };

    class Model : public IModel
    {
    public:
        Model(Core::IErrorMessanger& error_messanger, const Storage::IConfig& config);
        virtual ~Model(void) = default;
        virtual void add_sensor_data(const Storage::sensor_id_t& sensor_id, const Storage::axle_id_t& axle_id, const velocity_t& velocity, const timestamp_t& timespan) override final;
        virtual const Storage::sensor_ids_t& get_sensor_ids(void) const override final;
        virtual void process(void) override final;
        virtual const SensorResult& get_sensor_result(const Storage::sensor_id_t& sensor_id) const override final;
        virtual const OverallResult& get_overall_result(void) const override final;
        virtual timestamp_t get_max_absolute_timestamp(void) const override final;
        virtual std::pair<position_t, velocity_t> get_info_at_time(const timestamp_t& timestamp) const override final;
        virtual Storage::counters_t get_counters_at_time(const timestamp_t& timestamp) const override final;

    private:
        struct SensorData
        {
            SensorData(const Storage::axle_id_t& axle_id, const velocity_t& velocity, const timestamp_t& relative_timestamp);

            const Storage::axle_id_t axle_id;
            const velocity_t velocity;
            const timestamp_t relative_timestamp;
            timestamp_t absolute_timestamp;
            position_t relative_local_passed_distance;
            position_t relative_passed_distance;
            position_t absolute_head_position;
        };

        typedef std::list<SensorData> sensor_data_t;
        typedef std::list<std::pair<Storage::sensor_id_t, SensorData*>> sensor_data_seq_t;

        class Sensor
        {
        public:
            explicit Sensor(Core::IErrorMessanger& error_messanger);
            Sensor(Sensor&& other);
            SensorData& add_data(const Storage::axle_id_t& axle_id, const velocity_t& velocity, const timestamp_t& timespan);
            void process(void);
            void prepare_result(void);
            const SensorResult& get_result(void) const;
            timestamp_t get_delay(void) const;
            void set_delay(const timestamp_t& delay);

        private:
            Sensor(const Sensor&) = delete;
            Sensor& operator=(const Sensor&) = delete;

            Core::IErrorMessanger& m_error_messanger;
            sensor_data_t m_data;
            SensorResult m_result;
            timestamp_t m_delay;
        };

        typedef std::map<Storage::sensor_id_t, Sensor> sensors_t;

        Model(const Model&) = delete;
        Model(Model&&) = delete;
        Model& operator=(const Model&) = delete;
        Model& operator=(Model&&) = delete;

        const Sensor& get_sensor(const Storage::sensor_id_t& sensor_id) const;
        Sensor& get_sensor(const Storage::sensor_id_t& sensor_id);

        Core::IErrorMessanger& m_error_messanger;
        const Storage::IConfig& m_config;
        sensors_t m_sensors;
        Storage::sensor_ids_t m_sensor_ids;
        sensor_data_seq_t m_sequence;
        OverallResult m_overall_result;
        timestamp_t m_max_absolute_timestamp;
    };
}

#endif
