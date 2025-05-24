#ifndef ISAFE_H
#define ISAFE_H

#include "Types.h"
#include "IErrorMessanger.h"

#include <stdexcept>

namespace Core
{
    class ISafe
    {
    public:
        virtual ~ISafe(void) = default;

    protected:
        ISafe(void) = default;

        template <typename C, typename F, typename ... Args>
        void safe(C instance, IErrorMessanger& error_messanger, F f, Args&& ... args)
        {
            try
            {
                (instance->*f)(std::forward<Args&&>(args) ...);
            }
            catch (const std::runtime_error&)
            {
            }
            catch (...)
            {
                error_messanger.fire("Unknown exception.");
            }
        }

    private:
        ISafe(const ISafe&) = delete;
        ISafe(ISafe&&) = delete;
        ISafe& operator=(const ISafe&) = delete;
        ISafe& operator=(ISafe&&) = delete;
    };
}

#endif
