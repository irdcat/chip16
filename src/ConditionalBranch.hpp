#pragma once

#include "Types.hpp"

enum class ConditionalBranch : u16
{
    ZERO = 0,
    NOT_ZERO = 1,
    NEGATIVE = 2,
    NOT_NEGATIVE = 3,
    POSITIVE = 4,
    OVERFLOWED = 5,
    NOT_OVERFLOW = 6,
    ABOVE = 7,
    ABOVE_EQUAL = 8,
    BELOW = 9,
    BELOW_EQUAL = 10,
    GREATER = 11,
    GREATER_EQUAL = 12,
    LESS = 13,
    LESS_EQUAL = 14,
};