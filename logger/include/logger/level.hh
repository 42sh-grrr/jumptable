#pragma once

#include <logger/logger.hh>
#include <source_location>
#include <string>

namespace logger
{
    // clang-format off
    template <typename LOG>
    concept Loggable = requires(const LOG& log, std::ostream& os)
    {
        os << log;
    };
    // clang-format on

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
        friend class Stream;

        class Stream
        {
        public:
            Stream(const Level& level);

            template <Loggable LOG>
            const Stream& operator<<(const LOG& log) const;

        private:
            const Level& level_;
        };

        Level(Logger::log_level level, const std::string& color,
              const std::string& tag) noexcept;

        Logger::log_level get_level() const noexcept;

        const Stream operator()(const std::source_location location =
                                    std::source_location::current()) const;

    private:
        const Logger::log_level level_;
        const std::string color_;
        const std::string tag_;
    };

    const Level trace(0, GRAY, "TRA");
    const Level debug(1, BLUE, "DEB");
    const Level info(2, WHITE, "INF");
    const Level warn(3, YELLOW, "WAR");
    const Level error(4, RED, "ERR");
    const Level fatal(5, DARKRED, "FAT");
} // namespace logger

#include <logger/level.hxx>
