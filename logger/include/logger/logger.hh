#pragma once

#include <ostream>

namespace logger
{
    class Level;

    class Logger
    {
    public:
        using log_level = unsigned char;

        static void set_output_stream(std::ostream& os) noexcept;
        static std::ostream& get_output_stream() noexcept;

        static void set_level(Level level) noexcept;
        static void set_level(log_level level) noexcept;
        static log_level get_level() noexcept;

    private:
        static std::ostream* os_;
        static log_level level_;
    };
} // namespace logger

#include <logger/level.hh>
