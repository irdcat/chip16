#include "ConsoleLogStream.hpp"

void ConsoleLogStream::open()
{
    // NOP
}

void ConsoleLogStream::write(const std::string& msg)
{
    std::cout << msg << std::endl;
}

void ConsoleLogStream::close()
{
    // NOP
}
