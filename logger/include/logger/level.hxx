#pragma once

#include <logger/level.hh>

namespace logger
{
    template <Logger::log_level LEVEL>
    const std::string Level<LEVEL>::RESET = "\x1B[0m";

    template <Logger::log_level LEVEL>
    Level<LEVEL>::Level(const std::ostream& os, const Logger& logger,
                        const std::string& color)
        : os_(os)
        , logger_(logger)
        , color_(color)
    {}

    template <Logger::log_level LEVEL>
    template <Loggable LOG>
    Level<LEVEL>& Level<LEVEL>::operator<<(const LOG& log)
    {
        if (level >= log.get_level())
        {
            os_ << color_ << log << RESET;
        }
        return *this;
    }
} // namespace logger
