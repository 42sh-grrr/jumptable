#pragma once

#include <ostream>

namespace logger
{
    template <typename LOG>
    concept Loggable = requires(const LOG& log, const std::ostream& os)
    {
        os << log;
    };

    class Logger
    {
    public:
        using log_level = unsigned char;

        static void set_level(log_level level);

    private:
        static log_level level_;
    };
} // namespace logger
