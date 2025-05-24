#ifndef ICONFIG_H
#define ICONFIG_H

#include "Types.h"

namespace Storage
{
    class IConfig
    {
    public:
        virtual ~IConfig(void) = default;
        virtual const sensor_positions_t& get_positions(void) const = 0;

    protected:
        IConfig(void) = default;

    private:
        IConfig(const IConfig&) = delete;
        IConfig(IConfig&&) = delete;
        IConfig& operator=(const IConfig&) = delete;
        IConfig& operator=(IConfig&&) = delete;
    };
}

#endif
