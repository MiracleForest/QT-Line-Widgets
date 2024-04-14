/**
 * @author: Ticks
 * @email: ticks.cc@gmail.com
 */

#pragma once

#include <QtCore>
#include <array>
#include <chrono>
#include <cstdio>
#include <format>
#include <string>

namespace QLW
{
#define RGBSTR(r, g, b)   "\x1b[38;2;" #r ";" #g ";" #b "m"

#define QLW_LOG_TIMECOLOR RGBSTR(184, 217, 238)
#define QLW_LOG_FILECOLOR RGBSTR(126, 96, 54)
#define QLW_LOG_FUNCOLOR  RGBSTR(191, 164, 210)
#define QLW_LOG_NOCOLOR   "\x1b[0m"

#define LOG_LEVEL_MAP(XX)                            \
    XX(ALL, kALL, "ALL", RGBSTR(240, 240, 235))      \
    XX(INFO, kINFO, "INFO", RGBSTR(240, 240, 235))   \
    XX(DEBUG, kDEBUG, "DEBUG", RGBSTR(79, 155, 250)) \
    XX(WARN, kWARN, "WARN", RGBSTR(243, 187, 58))    \
    XX(ERROR, kERROR, "ERROR", RGBSTR(250, 86, 85))  \
    XX(OFF, kOFF, "", "\x1b[0m")

// Log Level
enum class LogLevel
{
    kNONE = 0,
#define XX(ID, NAME, DES, COLOR) NAME,
    LOG_LEVEL_MAP(XX)
#undef XX
}; // LogLevel

/**
 * @description  : get LogLevel string by LogLevel enum
 * @param         {LogLevel} level
 * @return        {const char*}
 */
inline constexpr const char* GetLogLevelStr(LogLevel level)
{
#define XX(ID, NAME, DES, COLOR) \
    case LogLevel::NAME:         \
        return DES;              \
        break;
    switch (level) {
        LOG_LEVEL_MAP(XX)
    default:
        return "Unknown";
    }
#undef XX
}

/**
 * @description  : get log color by LogLevel enum
 * @param         {LogLevel} level
 * @return        {const char*}
 */
inline constexpr auto GetLogLevelColor(LogLevel level)
{
#define XX(ID, NAME, DES, COLOR) \
    case LogLevel::NAME:         \
        return COLOR;            \
        break;
    switch (level) {
        LOG_LEVEL_MAP(XX)
    default:
        return "\x1b[0m";
    }
#undef XX
}

#undef LOG_LEVEL_MAP

// thread local global log level
static inline LogLevel __QLW_gLogLevel{LogLevel::kDEBUG};

inline void SetGlobalLogLevel(LogLevel level) { __QLW_gLogLevel = level; }
inline void CloseGlobalLog() { __QLW_gLogLevel = LogLevel::kOFF; }

class ConsoleLogger
{
public:
    static ConsoleLogger& GetInstance() noexcept
    {
        static thread_local ConsoleLogger g_console_logger{};
        return g_console_logger;
    }

    static ConsoleLogger New() noexcept { return ConsoleLogger{}; }
    static void SetMaxBufSize(unsigned long long max_size) noexcept
    {
        kMaxBufSize = max_size;
        GetInstance()._buffer.resize(kMaxBufSize);
    }
    ~ConsoleLogger() = default;

public:
    /**
     * @description  : print string with macro
     * @param         {LogLevel} log level
     * @param         {const char*} current file name
     * @param         {const char*} current function name
     * @param         {int} current file line
     * @param         {format_string} format string
     * @param         {const Args&...} format string args
     * @return        {void}
     */
    template <typename... Args>
    void __macroPrint(LogLevel level, const char* file_name,
                      const char* fun_name, int line,
                      const std::format_string<Args...> fmt,
                      const Args&... args)
    {
        if (level < __QLW_gLogLevel) {
            return;
        }
        constexpr auto fmt_start{"{}{}\x1b[0m {}{: <5s}\x1b[0m | {}<{: "
                                 ">10s}>\x1b[0m {}[{}:{}]\x1b[0m - "};
        setNowTimeBuffer();
        try {
            auto end = std::vformat_to(
                std::back_inserter(_buffer), fmt_start,
                std::make_format_args(QLW_LOG_TIMECOLOR, _time_buf.data(),
                                      GetLogLevelColor(level),
                                      GetLogLevelStr(level), QLW_LOG_FILECOLOR,
                                      file_name, QLW_LOG_FUNCOLOR, fun_name,
                                      line));

            std::vformat_to(end, fmt.get(), std::make_format_args(args...));
        } catch (const ::std::format_error& err) {
#ifdef DEBUG
            ::std::fputs(err.what(), stderr);
#endif
            return;
        }

        _buffer.push_back('\0');
        if (level < LogLevel::kERROR) {
            ::std::fputs(_buffer.data(), stdout);
        } else {
            ::std::fputs(_buffer.data(), stderr);
        }
        _buffer.clear();
    }
    /**
     * @description  : normat print
     * @param         {LogLevel} log level
     * @param         {format_string} format string
     * @param         {const Args&...} format string args
     * @return        {void}
     */
    template <typename... Args>
    void print(LogLevel level, const std::format_string<Args...> fmt,
               const Args&... args)
    {
        if (level < __QLW_gLogLevel) {
            return;
        }
        constexpr auto fmt_start{"{}{}\x1b[0m {}{: <5s}\x1b[0m - "};
        setNowTimeBuffer();
        try {
            auto end =
                std::format_to(std::back_inserter(_buffer), fmt_start,
                               QLW_LOG_TIMECOLOR, _time_buf.data(),
                               GetLogLevelColor(level), GetLogLevelStr(level));
            std::vformat_to(end, fmt.get(), std::make_format_args(args...));
        } catch (const ::std::format_error& exp) {
#ifdef DEBUG
            ::fputs(exp.what(), stderr);
#endif
            return;
        }
        _buffer.push_back('\0');
        if (level < LogLevel::kERROR) {
            std::fputs(_buffer.data(), stdout);
        } else {
            std::fputs(_buffer.data(), stderr);
        }
        _buffer.clear();
    }

private:
    using Millisecond = ::std::chrono::milliseconds; // ms
    using Second = ::std::chrono::seconds;           // s

    ConsoleLogger() noexcept
    {
        _buffer.reserve(kMaxBufSize);
        std::fill(_time_buf.begin(), _time_buf.end(), '\0');
    }

    void setNowTimeCount() noexcept
    {
        auto now_time = std::chrono::system_clock::now();
        _now_time_ms = std::chrono::duration_cast<Millisecond>(
            now_time.time_since_epoch());
        _now_time_s =
            std::chrono::duration_cast<Second>(now_time.time_since_epoch());
    }
    void setNowTimeBuffer() noexcept
    {
        auto ms = _now_time_ms.count();
        auto s = _now_time_s.count();
        setNowTimeCount();
        auto now_s = _now_time_s.count();
        if (now_s - s >= 60) {
            std::tm tm_now{};
#if defined(Q_OS_WIN32)
            ::localtime_s(std::addressof(tm_now), std::addressof(now_s));
#elif define(Q_OS_LINUX)
            ::localtime_r(std::addressof(now_s), std::addressof(tm_now));
#endif
            auto ms_arg = (_now_time_ms % 1000).count();
            std::format_to_n(_time_buf.data(), 18,
                             "{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d}",
                             tm_now.tm_mon + 1, tm_now.tm_mday, tm_now.tm_hour,
                             tm_now.tm_min, tm_now.tm_sec, ms_arg);
        } else {
            if (s == now_s) {
                std::format_to_n(_time_buf.data() + 15, 3, "{:3}",
                                 (_now_time_ms % 1000).count());
            } else {
                std::format_to_n(_time_buf.data() + 12, 6, "{:2}.{:3}",
                                 (_now_time_s % 60).count(),
                                 (_now_time_ms % 1000).count());
            }
        }
    }

private:
    static inline unsigned long long kMaxBufSize{512}; // max buffer size
    ::std::string _buffer;                             // log buffer
    Millisecond _now_time_ms{0};                       // now time ms
    Second _now_time_s{0};                             // now time s
    // 01-01 17:01:30.203
    ::std::array<char, 19> _time_buf{}; // now time buffer

}; // ConsoleLogger

// Some Log Macro Define
#if defined(Q_CC_MSVC)
#define LOGI(MSG, ...)                         \
    ConsoleLogger::GetInstance().__macroPrint( \
        LogLevel::kINFO, __FILE__, __func__, __LINE__, MSG "\n", __VA_ARGS__)
#define LOGD(MSG, ...)                         \
    ConsoleLogger::GetInstance().__macroPrint( \
        LogLevel::kDEBUG, __FILE__, __func__, __LINE__, MSG "\n", __VA_ARGS__)
#define LOGW(MSG, ...)                         \
    ConsoleLogger::GetInstance().__macroPrint( \
        LogLevel::kWARN, __FILE__, __func__, __LINE__, MSG "\n", __VA_ARGS__)
#define LOGE(MSG, ...)                         \
    ConsoleLogger::GetInstance().__macroPrint( \
        LogLevel::kERROR, __FILE__, __func__, __LINE__, MSG "\n", __VA_ARGS__)
#else
#define LOGI(MSG, ...)                                 \
    ConsoleLogger::GetInstance().__macroPrint(         \
        LogLevel::kINFO, __FILE__, __func__, __LINE__, \
        MSG "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOGD(MSG, ...)                                  \
    ConsoleLogger::GetInstance().__macroPrint(          \
        LogLevel::kDEBUG, __FILE__, __func__, __LINE__, \
        MSG "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOGW(MSG, ...)                                 \
    ConsoleLogger::GetInstance().__macroPrint(         \
        LogLevel::kWARN, __FILE__, __func__, __LINE__, \
        MSG "\n" __VA_OPT__(, ) __VA_ARGS__)
#define LOGE(MSG, ...)                                  \
    ConsoleLogger::GetInstance().__macroPrint(          \
        LogLevel::kERROR, __FILE__, __func__, __LINE__, \
        MSG "\n" __VA_OPT__(, ) __VA_ARGS__)
#endif

// easy to print messages
template <typename... Args>
inline void Print(::std::format_string<Args...> fmt, const Args&... args)
{
    static thread_local ::std::string __QLW_gPrintBuffer;
    try {
        std::vformat_to(std::back_inserter(__QLW_gPrintBuffer), fmt.get(),
                        std::make_format_args(args...));
    } catch (const ::std::format_error& exp) {
#ifdef DEBUG
        ::fputs(exp.what(), stderr);
#endif
        return;
    }
    __QLW_gPrintBuffer.push_back('\0');
    std::fputs(__QLW_gPrintBuffer.data(), stdout);
    __QLW_gPrintBuffer.clear();
}

template <typename... Args>
inline void Println(::std::format_string<Args...> fmt, const Args&... args)
{
    static thread_local ::std::string __QLW_gPrintBuffer;
    std::string fmt_view{std::move(fmt.get())};
    fmt_view.push_back('\n');
    try {
        std::vformat_to(std::back_inserter(__QLW_gPrintBuffer), fmt_view,
                        std::make_format_args(args...));
    } catch (const ::std::format_error& exp) {
#ifdef DEBUG
        ::fputs(exp.what(), stderr);
#endif
        return;
    }
    __QLW_gPrintBuffer.push_back('\0');
    std::fputs(__QLW_gPrintBuffer.data(), stdout);
    __QLW_gPrintBuffer.clear();
}

} // namespace QLW