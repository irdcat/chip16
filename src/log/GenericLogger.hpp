#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

template <class StreamType>
class GenericLogger
{
public:
    GenericLogger(const std::string& name);

    template <typename ...Args>
    void debug(Args ...args);

    template <typename ...Args>
    void info(Args ...args);

    template <typename ...Args>
    void warn(Args ...args);

    template <typename ...Args>
    void error(Args ...args);

private:
    enum Severity 
    {
        DEBUG, 
        INFO, 
        WARN, 
        ERROR
    };

    template <Severity LogSeverity, typename ...Args>
    void print(Args ... args);

    template <typename First, typename ... Args>
    void printImpl(First first, Args ... args);

    void printImpl();

    const std::string createHeader();
    const std::string getTimeAsString();

    std::stringstream mStringStream;
    std::string mName;

    static std::unique_ptr<StreamType> logStream;
};

template<class StreamType>
std::unique_ptr<StreamType> GenericLogger<StreamType>::logStream = std::make_unique<StreamType>();

template<class StreamType>
inline GenericLogger<StreamType>::GenericLogger(const std::string& name)
{
    mName = name;
}

template<class StreamType>
inline const std::string GenericLogger<StreamType>::createHeader()
{
    std::stringstream headerStream;
    headerStream.str("");
    headerStream << getTimeAsString();
    headerStream << ' ' << '[' << mName << ']';
    return headerStream.str();
}

template<class StreamType>
inline const std::string GenericLogger<StreamType>::getTimeAsString()
{
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss.str("");
    struct tm tm;
    localtime_s(&tm, &nowTimeT);
    ss << std::put_time(&tm, "%Y-%m-%d_%X");
    return ss.str();
}

template<class StreamType>
template<typename ...Args>
inline void GenericLogger<StreamType>::debug(Args ...args)
{
    this->print<Severity::DEBUG>(std::forward<Args>(args)...);
}

template<class StreamType>
template<typename ...Args>
inline void GenericLogger<StreamType>::info(Args ...args)
{
    this->print<Severity::INFO>(std::forward<Args>(args)...);
}

template<class StreamType>
template<typename ...Args>
inline void GenericLogger<StreamType>::warn(Args ...args)
{
    this->print<Severity::WARN>(std::forward<Args>(args)...);
}

template<class StreamType>
template<typename ...Args>
inline void GenericLogger<StreamType>::error(Args ...args)
{
    this->print<Severity::ERROR>(std::forward<Args>(args)...);
}

template<class StreamType>
template<typename GenericLogger<StreamType>::Severity LogSeverity, typename ...Args>
inline void GenericLogger<StreamType>::print(Args ...args)
{
    mStringStream << createHeader();
    if constexpr (LogSeverity == Severity::DEBUG)
        mStringStream << " [DEBUG] ";
    else if (LogSeverity == Severity::INFO)
        mStringStream << " [INFO] ";
    else if (LogSeverity == Severity::WARN)
        mStringStream << " [WARN] ";
    else if (LogSeverity == Severity::ERROR)
        mStringStream << " [ERROR] ";

    printImpl(args...);
}

template<class StreamType>
template<typename First, typename ...Args>
inline void GenericLogger<StreamType>::printImpl(First first, Args ...args)
{
    mStringStream << first;
    printImpl(args...);
}

template<class StreamType>
inline void GenericLogger<StreamType>::printImpl()
{
    logStream->write(createHeader() + mStringStream.str());
    mStringStream.str("");
}