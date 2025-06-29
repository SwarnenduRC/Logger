#ifndef LOGGER_MACROS_HPP
#define LOGGER_MACROS_HPP

#include "LogHelper.hpp"

namespace logger
{
    #define LOG_ENTRY(fmt_str, __VA_ARGS__...)                                   \
    log_entry(__FILE__, __PRETTY_FUNCTION__, __LINE__, #fmt_str, ##__VA_ARGS__);  \

    #define LOG_EXIT(fmt_str, __VA_ARGS__...)                                   \
    log_exit(__FILE__, __PRETTY_FUNCTION__, __LINE__, #fmt_str, ##__VA_ARGS__);  \

    #define LOG_INFO(fmt_str, __VA_ARGS__...)                                   \
    log_info(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str, ##__VA_ARGS__);   \

    #define LOG_IMP(fmt_str, __VA_ARGS__...)                                    \
    log_info(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str, ##__VA_ARGS__);   \

    #define LOG_WARN(fmt_str, __VA_ARGS__...)                                   \
    log_warn(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str, ##__VA_ARGS__);   \

    #define LOG_ERR(fmt_str, __VA_ARGS__...)                                   \
    log_err(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str, ##__VA_ARGS__);   \

    #define LOG_DBG(fmt_str, __VA_ARGS__...)                                   \
    log_dbg(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str, ##__VA_ARGS__);   \

    #define LOG_ASSERT(cond, fmt_str...)                                               \
    do                                                                                  \
    {                                                                                    \
        if (!(cond))                                                                      \
            log_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #cond, true, #fmt_str);    \
    } while (0);                                                                            \

    #define LOG_ASSERT_MSG(cond, fmt_str, __VA_ARGS__...)                                          \
    do                                                                                              \
    {                                                                                                \
        if (!(cond))                                                                                  \
            log_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #cond, true, fmt_str, ##__VA_ARGS__);  \
    } while (0);                                                                                        \

    #define LOG_FATAL(fmt_str, __VA_ARGS__...)                                   \
    log_fatal(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str, ##__VA_ARGS__);   \

} // namespace logger

#endif  //LOGGER_MACROS_HPP