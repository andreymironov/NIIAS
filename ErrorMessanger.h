#ifndef ERRORMESSANGER_H
#define ERRORMESSANGER_H

#include "IErrorMessanger.h"
#include "IWindow.h"

namespace Core
{
    class ErrorMessanger : public IErrorMessanger
    {
    public:
        explicit ErrorMessanger(IWindow& window);
        virtual ~ErrorMessanger(void) = default;
        virtual void fire(const error_message_t& error_message) const override final;

    private:
        ErrorMessanger(const ErrorMessanger&) = delete;
        ErrorMessanger(ErrorMessanger&&) = delete;
        ErrorMessanger& operator=(const ErrorMessanger&) = delete;
        ErrorMessanger& operator=(ErrorMessanger&&) = delete;

        IWindow& m_window;
    };
}

#endif
