#pragma once

#include <logger/logger.hh>
#include <string>

namespace logger
{
    template <Logger::log_level LEVEL>
    class Level
    {
    public:
        static constexpr Logger::log_level level = LEVEL;

        Level(const std::ostream& os, const Logger& logger, const std::string& color);

        template <Loggable LOG>
        Level& operator<<(const LOG& log);

    private:
        static const std::string RESET;

    private:
        const std::ostream& os_;
        const Logger& logger_;
        const std::string color_;
    };
} // namespace logger

#include <logger/level.hxx>
