#pragma once

enum ModificatorType
{
    HEX
};

template <typename T, ModificatorType Modificator>
class OutputModificator
{
public:
    OutputModificator(T value);

    ModificatorType getType() const;

    T getValue() const;

private:
    T value;
};

template<typename T, ModificatorType Modificator>
inline OutputModificator<T, Modificator>::OutputModificator(T value)
    : value(value)
{
}

template<typename T, ModificatorType Modificator>
inline ModificatorType OutputModificator<T, Modificator>::getType() const
{
    return Modificator;
}

template<typename T, ModificatorType Modificator>
inline T OutputModificator<T, Modificator>::getValue() const
{
    return value;
}

template <typename T>
using HexModificator = OutputModificator<T, ModificatorType::HEX>;

template <typename T>
inline HexModificator<T> logHex(T value)
{
    return HexModificator<T>(value);
}
