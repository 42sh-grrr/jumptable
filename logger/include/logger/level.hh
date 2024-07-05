#pragma once

#include <logger/logger.hh>
#include <string>

namespace logger
{
    template <typename LOG>
    concept Loggable =
        requires(const LOG& log, std::ostream& os) { os << log; };

    const std::string RESET = "\x1B[0m";
    const std::string GRAY = "\x1B[90m";
    const std::string BLUE = "\x1B[94m";
    const std::string WHITE = "\x1B[0m";
    const std::string YELLOW = "\x1B[93m";
    const std::string RED = "\x1B[91m";
    const std::string DARKRED = "\x1B[1m\x1B[31m";

    class Level
    {
    public:
        Level(Logger::log_level level, const std::string& color);

        Logger::log_level get_level() const;

        template <Loggable LOG>
        const Level& operator<<(const LOG& log) const;

    private:
        const Logger::log_level level_;
        const std::string color_;
    };

    const Level trace(0, GRAY);
    const Level debug(1, BLUE);
    const Level info(2, WHITE);
    const Level warn(3, YELLOW);
    const Level error(4, RED);
    const Level fatal(5, DARKRED);
} // namespace logger

#include <logger/level.hxx>
