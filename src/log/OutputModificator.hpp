#pragma once

#include <sstream>

template <typename T>
class LogModificator
{
public:
    LogModificator(T value);

    virtual void print(std::ostream& os) const = 0;

private:
    T value;
};

template <typename T>
LogModificator::LogModificator(T value)
    : value(value)
{
}