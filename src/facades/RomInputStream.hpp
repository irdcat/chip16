#pragma once

#include <istream>

/**
 * Additional layer of abstraction holding input stream.
 */
class RomInputStream
{
public:
    virtual ~RomInputStream() = default;

    virtual std::istream& getStream() = 0;
};