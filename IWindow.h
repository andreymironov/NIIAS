#ifndef IWINDOW_H
#define IWINDOW_H

#include "Types.h"

namespace Core
{
    class IWindow
    {
    public:
        virtual ~IWindow(void) = default;
        virtual void add_sensor_line(const x_string_t& line) = 0;
        virtual void clear_sensor_lines(void) = 0;
        virtual void add_error_line(const x_string_t& line) = 0;
        virtual void clear_error_lines(void) = 0;
        virtual void add_sensor_name(const x_string_t& name) = 0;
        virtual void clear_sensor_names(void) = 0;
        virtual void set_sensor_title(const x_string_t& title) = 0;
        virtual void set_sensor_chasis_formula(const x_string_t& formula) = 0;
        virtual void add_sensor_info_line(const x_string_t& line) = 0;
        virtual void set_sensor_average_velocity(const x_string_t& average_velocity) = 0;
        virtual void set_sensor_velocity_deviation(const x_string_t& velocity_deviation) = 0;
        virtual void clear_sensor_info(void) = 0;
        virtual void set_overall_chasis_formula(const x_string_t& formula) = 0;
        virtual void set_overall_length_formula(const x_string_t& formula) = 0;
        virtual void clear_overall_info(void) = 0;

    protected:
        IWindow(void) = default;

    private:
        IWindow(const IWindow&) = delete;
        IWindow(IWindow&&) = delete;
        IWindow& operator=(const IWindow&) = delete;
        IWindow& operator=(IWindow&&) = delete;
    };
}

#endif
