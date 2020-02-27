#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../src/facades/RomInputStream.hpp"

class RomInputStreamMock : public RomInputStream
{
public:
    MOCK_METHOD0(getStream, std::istream& ());
};