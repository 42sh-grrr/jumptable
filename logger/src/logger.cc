#include <logger/logger.hh>

namespace logger
{
    Logger::log_level Logger::level_ = 0;

    void Logger::set_level(Logger::log_level level)
    {
        level_ = level;
    }
} // namespace logger
