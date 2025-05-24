#ifndef IMODEL_H
#define IMODEL_H

#include "Types.h"

namespace Math
{
    class IModel
    {
    public:
        virtual ~IModel(void) = default;
        virtual void add_sensor_data(const Storage::sensor_id_t& sensor_id, const Storage::axle_id_t& axle_id, const Math::velocity_t& velocity, const Math::timestamp_t& timespan) = 0;
        virtual const Storage::sensor_ids_t& get_sensor_ids(void) const = 0;
        virtual void process(void) = 0;
        virtual const SensorResult& get_sensor_result(const Storage::sensor_id_t& sensor_id) const = 0;
        virtual const OverallResult& get_overall_result(void) const = 0;
        virtual timestamp_t get_max_absolute_timestamp(void) const = 0;
        virtual std::pair<position_t, velocity_t> get_info_at_time(const timestamp_t& timestamp) const = 0;
        virtual Storage::counters_t get_counters_at_time(const timestamp_t& timestamp) const = 0;

    protected:
        IModel(void) = default;

    private:
        IModel(const IModel&) = delete;
        IModel(IModel&&) = delete;
        IModel& operator=(const IModel&) = delete;
        IModel& operator=(IModel&&) = delete;
    };
}

#endif
