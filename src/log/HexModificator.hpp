#pragma once

#include <memory>
#include <iomanip>
#include <type_traits>
#include "LogModificator.hpp"

template<typename T>
class HexModificator : public LogModificator<T>
{
public:
    static_assert(std::is_integral_v<T>, "Type of T must be integral!");

    HexModificator(T value);

    ~HexModificator() = default;

    void apply(std::ostream& os) const override;
};

template<typename T>
inline HexModificator<T>::HexModificator(T value)
    : LogModificator<T>(value)
{
}

template<typename T>
inline void HexModificator<T>::apply(std::ostream& os) const
{
    os << "0x" << std::uppercase << std::hex << this->value;
}

template<typename T>
inline std::shared_ptr<LogModificator<T>> logHex(T value)
{
    return std::make_shared<HexModificator<T>>(value);
}