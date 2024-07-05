#include <iostream>
#include <logger/level.hh>
#include <logger/logger.hh>

namespace logger
{
    std::ostream* Logger::os_ = &std::clog;
    Logger::log_level Logger::level_ = 0;

    void Logger::set_output_stream(std::ostream& os)
    {
        os_ = &os;
    }

    std::ostream& Logger::get_output_stream()
    {
        return *os_;
    }

    void Logger::set_level(Level level)
    {
        level_ = level.get_level();
    }

    void Logger::set_level(Logger::log_level level)
    {
        level_ = level;
    }

    Logger::log_level Logger::get_level()
    {
        return level_;
    }
} // namespace logger
