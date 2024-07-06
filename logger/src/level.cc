#include <logger/level.hh>
#include <source_location>
#include <sstream>

namespace logger
{
    namespace
    {
        const std::string split_file(const std::string& file)
        {
            std::ostringstream oss;
            std::size_t file_part = file.find_last_of('/');
            std::string path = file.substr(0, file_part);
            std::string file_name = file.substr(file_part);

            for (auto it = path.begin(); it != path.end(); ++it)
            {
                if (*it == '/')
                {
                    oss << '/' << *(it + 1);
                }
            }

            oss << file_name;
            return oss.str();
        }
    } // namespace

    Level::Level(Logger::log_level level, const std::string& color,
                 const std::string& tag) noexcept
        : level_(level)
        , color_(color)
        , tag_(tag)
    {}

    Level::Stream::Stream(const Level& level)
        : level_(level)
    {}

    Logger::log_level Level::get_level() const noexcept
    {
        return level_;
    }

    const Level::Stream
    Level::operator()(const std::source_location location) const
    {
        std::ostringstream oss;
        oss << '[' << tag_ << "] " << split_file(location.file_name()) << '('
            << location.line() << ") ";
        Stream ls(*this);
        ls << oss.str();
        return ls;
    }
} // namespace logger
