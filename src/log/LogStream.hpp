#pragma once

#include <string>

class LogStream
{
public:
    virtual ~LogStream() = default;

    virtual void open() = 0;

    virtual void close() = 0;

    virtual void write(const std::string& msg) = 0;
};