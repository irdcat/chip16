#pragma once

#include <ostream>

template <typename T>
class LogModificator
{
public:
    LogModificator(T value);

    virtual ~LogModificator() = default;

    virtual void apply(std::ostream& os) const = 0;

protected:
    T value;
};

template <typename T>
LogModificator<T>::LogModificator(T value)
    : value(value)
{
}
