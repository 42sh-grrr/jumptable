#include <logger/level.hh>

namespace logger
{
    Level::Level(Logger::log_level level, const std::string& color)
        : level_(level)
        , color_(color)
    {}

    Logger::log_level Level::get_level() const
    {
        return level_;
    }
} // namespace logger
