#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <iostream>
#include <logger/logger.hh>

#define STRINGIFY(X) #X

#define LoggerTest(Level, Color, Tag)                                          \
    Test(liblogger, logger_##Level, .init = cr_redirect_stderr)                \
    {                                                                          \
        std::source_location location = std::source_location::current();       \
        const std::string log = "Hello, world!\n";                             \
        std::ostringstream oss;                                                \
        oss << logger::Color << '[' << STRINGIFY(Tag) "] "                     \
            << split_file(location.file_name()) << '(' << location.line()      \
            << ") " << logger::RESET << logger::Color << log << logger::RESET; \
        const std::string expected = oss.str();                                \
        logger::Level() << log;                                                \
        cr_expect_stderr_eq_str(expected.c_str());                             \
    }

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

TestSuite(liblogger);

LoggerTest(trace, GRAY, TRA);
LoggerTest(debug, BLUE, DEB);
LoggerTest(info, WHITE, INF);
LoggerTest(warn, YELLOW, WAR);
LoggerTest(error, RED, ERR);
LoggerTest(fatal, DARKRED, FAT);

Test(liblogger, logger_no_info, .init = cr_redirect_stderr)
{
    logger::Logger::set_level(logger::warn);

    const std::string log = "Hello, world!\n";
    const std::string expected = "";
    logger::info() << log;
    cr_expect_stderr_eq_str(expected.c_str());
}

Test(liblogger, logger_cout_info, .init = cr_redirect_stdout)
{
    logger::Logger::set_output_stream(std::cout);

    std::source_location location = std::source_location::current();
    const std::string log = "Hello, world!\n";
    std::ostringstream oss;
    oss << logger::WHITE << "[INF] " << split_file(location.file_name()) << '('
        << (location.line() + 7) << ") " << logger::RESET << logger::WHITE
        << log << logger::RESET;
    const std::string expected = oss.str();
    logger::info() << log;

    std::cout.flush();

    cr_expect_stdout_eq_str(expected.c_str());
}

Test(liblogger, logger_cout_no_info, .init = cr_redirect_stdout)
{
    logger::Logger::set_output_stream(std::cout);
    logger::Logger::set_level(logger::warn);

    const std::string log = "Hello, world!\n";
    const std::string expected = "";
    logger::info() << log;

    std::cout.flush();

    cr_expect_stdout_eq_str(expected.c_str());
}
