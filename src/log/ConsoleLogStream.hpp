#pragma once

#include "LogStream.hpp"
#include <iostream>

class ConsoleLogStream : public LogStream
{
public:
    ConsoleLogStream() = default;

    ~ConsoleLogStream() = default;

    void open() override;

    void write(const std::string& msg) override;

    void close() override;
};