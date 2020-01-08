#pragma once

#include <fstream>
#include "RomInputStream.hpp"

class RomFileInputStream : public RomInputStream
{
public:
    RomFileInputStream(std::string filename);

    ~RomFileInputStream() = default;

    std::istream& getStream() override;

private:
    std::ifstream inputRomStream;
};