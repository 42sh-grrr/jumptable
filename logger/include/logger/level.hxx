#pragma once

#include <logger/level.hh>

namespace logger
{
    template <Loggable LOG>
    const Level& Level::operator<<(const LOG& log) const
    {
        if (level_ >= Logger::get_level())
        {
            Logger::get_output_stream() << color_ << log << RESET;
        }
        return *this;
    }
}; // namespace logger
