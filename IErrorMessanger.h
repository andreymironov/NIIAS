#ifndef IERRORMESSANGER_H
#define IERRORMESSANGER_H

#include "Types.h"

namespace Core
{
    class IErrorMessanger
    {
    public:
        virtual ~IErrorMessanger(void) = default;
        virtual void fire(const error_message_t& error_message) const = 0;

    protected:
        IErrorMessanger(void) = default;

    private:
        IErrorMessanger(const IErrorMessanger&) = delete;
        IErrorMessanger(IErrorMessanger&&) = delete;
        IErrorMessanger& operator=(const IErrorMessanger&) = delete;
        IErrorMessanger& operator=(IErrorMessanger&&) = delete;
    };
}

#endif
