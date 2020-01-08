#pragma once

#include <string>
#include <memory>
#include "RomInputStream.hpp"

class RomFacade
{
public:
    virtual ~RomFacade() = default;

    virtual bool loadRomIntoMemory(const std::shared_ptr<RomInputStream>& romInputStream) = 0;
};