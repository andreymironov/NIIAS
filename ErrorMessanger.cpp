#include "ErrorMessanger.h"

#include <stdexcept>

namespace Core
{
    ErrorMessanger::ErrorMessanger(IWindow& window) :
        m_window(window)
    {
    }

    void ErrorMessanger::fire(const error_message_t& error_message) const
    {
        m_window.add_error_line(x_string_t(error_message.cbegin(), error_message.cend()));
        throw std::runtime_error(error_message);
    }
}
