#pragma once

#include "Types.hpp"

union ControllerState
{
    u16 raw;
    struct
    {
        u8 up : 1;
        u8 down : 1;
        u8 left : 1;
        u8 right : 1;
        u8 select : 1;
        u8 start : 1;
        u8 A : 1;
        u8 B : 1;
        u8 : 8;
    };
};