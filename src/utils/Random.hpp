#pragma once

#include <random>
#include <type_traits>

class Random
{
public:
    template <typename IntType, std::enable_if_t<std::is_integral_v<IntType>, IntType> = 0>
    static IntType get(IntType min, IntType max)
    {
        std::uniform_int_distribution<IntType> dist(min, max);
        return dist(randomEngine);
    }

private:
    static std::default_random_engine randomEngine;
};