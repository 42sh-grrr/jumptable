#pragma once

#include <ostream>

namespace logger
{
    class Level;

    class Logger
    {
    public:
        using log_level = unsigned char;

        static void init();

        static void set_output_stream(std::ostream& os);
        static std::ostream& get_output_stream();

        static void set_level(Level level);
        static void set_level(log_level level);
        static log_level get_level();

    private:
        static std::ostream* os_;
        static log_level level_;
    };
} // namespace logger

#include <logger/level.hh>
