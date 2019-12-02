#pragma once

#include <utility>

#include "GenericLogger.hpp"
#include "ConsoleLogStream.hpp"

#define STRINGIFY(className) #className

class Logger
{
public:
    Logger(const std::string& name);

    template <typename ...Args>
    void debug(Args ...args);

    template <typename ...Args>
    void info(Args ...args);

    template <typename ...Args>
    void warn(Args ...args);

    template <typename ...Args>
    void error(Args ...args);

private:
    GenericLogger<ConsoleLogStream> mConsoleLogger;
};

template<typename ...Args>
inline void Logger::debug(Args ...args)
{
    mConsoleLogger.debug(std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::info(Args ...args)
{
    mConsoleLogger.info(std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::warn(Args ...args)
{
    mConsoleLogger.warn(std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::error(Args ...args)
{
    mConsoleLogger.error(std::forward<Args>(args)...);
}
