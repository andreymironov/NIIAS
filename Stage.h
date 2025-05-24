#ifndef STAGE_H
#define STAGE_H

#include "IWindow.h"
#include "ISafe.h"
#include "IFileReader.h"
#include "IConfig.h"
#include "Parser.h"
#include "Model.h"

namespace Model
{
    class Stage : public Core::ISafe
    {
    public:
        Stage(Core::IWindow& window, Core::IErrorMessanger& error_messanger, Core::IFileReader& file_reader, Storage::IParser& parser, const Core::x_string_t& file_path, const Storage::IConfig& config);
        ~Stage(void);
        void on_sensor_selection(const size_t index);
        void on_time_selection(const bool is_relative);
        const Math::OverallResult& get_overall_result(void) const;
        Math::timestamp_t get_max_absolute_timestamp(void) const;
        std::pair<Math::position_t, Math::velocity_t> get_info_at_time(const Math::timestamp_t& timestamp) const;
        Storage::counters_t get_counters_at_time(const Math::timestamp_t& timestamp) const;

    private:
        Stage(const Stage&) = delete;
        Stage(Stage&&) = delete;
        Stage& operator=(const Stage&) = delete;
        Stage& operator=(Stage&&) = delete;

        void prepare(void);
        void on_sensor_selection_helper(const size_t index);
        void on_time_selection_helper(const bool is_relative);

        Core::IWindow& m_window;
        Core::IErrorMessanger& m_error_messanger;
        Core::IFileReader& m_file_reader;
        Storage::IParser& m_parser;
        const Core::x_string_t m_file_path;
        const Storage::IConfig& m_config;
        Math::Model m_model;
        size_t m_selected_sensor_index;
        bool m_is_time_relative;
    };
}

#endif
