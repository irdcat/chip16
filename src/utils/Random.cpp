#include "Random.hpp"

std::default_random_engine Random::randomEngine{ std::random_device{}() };