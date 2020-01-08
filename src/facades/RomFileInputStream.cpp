#include "RomFileInputStream.hpp"

RomFileInputStream::RomFileInputStream(std::string filename)
    : inputRomStream(filename, std::ios::binary)
{
}

std::istream& RomFileInputStream::getStream()
{
    return inputRomStream;
}
