#ifndef LOGGER_MACROS_HPP
#define LOGGER_MACROS_HPP

#include "LogHelper.hpp"

namespace logger
{
    #define LOG_ENTRY(fmt_str, ...)                                                         \
    log_entry(__FILE__, __PRETTY_FUNCTION__, __LINE__, #fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    #define LOG_EXIT(fmt_str, ...)                                                          \
    log_exit(__FILE__, __PRETTY_FUNCTION__, __LINE__, #fmt_str __VA_OPT__(,) __VA_ARGS__);   \

    #define LOG_INFO(fmt_str, ...)                                                         \
    log_info(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);   \

    #define LOG_IMP(fmt_str, ...)                                                          \
    log_info(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);   \

    #define LOG_WARN(fmt_str, ...)                                                        \
    log_warn(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    #define LOG_ERR(fmt_str, ...)                                                        \
    log_err(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    #define LOG_DBG(fmt_str, ...)                                                        \
    log_dbg(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    #define LOG_ASSERT(cond, ...)                                                                          \
    do                                                                                                      \
    {                                                                                                        \
        if (!(cond))                                                                                          \
            log_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #cond, true, #cond __VA_OPT__(,) __VA_ARGS__); \
    } while (0);                                                                                                \

    #define LOG_ASSERT_MSG(cond, fmt_str, ...)                                                               \
    do                                                                                                        \
    {                                                                                                          \
        if (!(cond))                                                                                            \
            log_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #cond, true, fmt_str __VA_OPT__(,) __VA_ARGS__); \
    } while (0);                                                                                                  \

    #define LOG_FATAL(fmt_str, ...)                                                        \
    log_fatal(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

} // namespace logger

#endif  //LOGGER_MACROS_HPP