#pragma once

#include <memory>
#include <type_traits>
#include "LogModificator.hpp"

template<typename T>
class NumberModificator : public LogModificator<T>
{
public:
    static_assert(std::is_integral_v<T>, "Type of T must be integral!");

    NumberModificator(T value);

    ~NumberModificator() = default;

    void apply(std::ostream& os) const override;
};

template<typename T>
inline NumberModificator<T>::NumberModificator(T value)
    : LogModificator<T>(value)
{
}

template<typename T>
inline void NumberModificator<T>::apply(std::ostream& os) const
{
    if constexpr (std::is_signed_v<T>)
        os << static_cast<signed>(this->value);
    else
        os << static_cast<unsigned>(this->value);
}

template <typename T>
inline std::shared_ptr<LogModificator<T>> logNumber(T value)
{
    return std::make_shared<NumberModificator<T>>(value);
}