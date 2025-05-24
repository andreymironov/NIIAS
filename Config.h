#ifndef CONFIG_H
#define CONFIG_H

#include "IConfig.h"
#include "ISafe.h"
#include "IFileReader.h"
#include "Parser.h"

namespace Storage
{
    class Config : public IConfig, public Core::ISafe
    {
    public:
        Config(Core::IErrorMessanger& error_messanger, Core::IFileReader& file_reader, IParser& parser, const Core::path_t& file_path);
        virtual ~Config(void) = default;
        virtual const sensor_positions_t& get_positions(void) const override final;

    private:
        Config(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(const Config&) = delete;
        Config& operator=(Config&&) = delete;

        void prepare(void);
        Math::position_t get_position(const sensor_id_t& sensor_id);

        Core::IErrorMessanger& m_error_messanger;
        Core::IFileReader& m_file_reader;
        IParser& m_parser;
        const Core::path_t& m_file_path;
        config_records_t m_config_records;
        sensor_positions_t m_sensor_positions;
    };
}

#endif
