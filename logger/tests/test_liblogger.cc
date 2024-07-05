#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <iostream>
#include <logger/logger.hh>

#include "logger/level.hh"

#define LoggerTest(Level, Color)                                               \
    Test(liblogger, logger_##Level, .init = cr_redirect_stderr)                \
    {                                                                          \
        const std::string log = "Hello, world!\n";                             \
        const std::string expected = logger::Color + log + logger::RESET;      \
        logger::Level << log;                                                  \
        cr_expect_stderr_eq_str(expected.c_str());                             \
    }

TestSuite(liblogger);

LoggerTest(trace, GRAY);
LoggerTest(debug, BLUE);
LoggerTest(info, WHITE);
LoggerTest(warn, YELLOW);
LoggerTest(error, RED);
LoggerTest(fatal, DARKRED);

Test(liblogger, logger_no_info, .init = cr_redirect_stderr)
{
    logger::Logger::set_level(logger::warn);

    const std::string log = "Hello, world!\n";
    const std::string expected = "";
    logger::info << log;
    cr_expect_stderr_eq_str(expected.c_str());
}

Test(liblogger, logger_cout_info, .init = cr_redirect_stdout)
{
    logger::Logger::set_output_stream(std::cout);

    const std::string log = "Hello, world!\n";
    const std::string expected = logger::WHITE + log + logger::RESET;
    logger::info << log;

    std::cout.flush();

    cr_expect_stdout_eq_str(expected.c_str());
}

Test(liblogger, logger_cout_no_info, .init = cr_redirect_stdout)
{
    logger::Logger::set_output_stream(std::cout);
    logger::Logger::set_level(logger::warn);

    const std::string log = "Hello, world!\n";
    const std::string expected = "";
    logger::info << log;

    std::cout.flush();

    cr_expect_stdout_eq_str(expected.c_str());
}
