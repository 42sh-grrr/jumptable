#pragma once

#include <logger/level.hh>

namespace logger
{
    template <Loggable LOG>
    const Level::Stream& Level::Stream::operator<<(const LOG& log) const
    {
        if (level_.level_ >= Logger::get_level())
        {
            Logger::get_output_stream() << level_.color_ << log << RESET;
        }
        return *this;
    }
}; // namespace logger
