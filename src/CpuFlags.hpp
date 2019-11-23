#pragma once

#include "Types.hpp"

union CpuFlags
{
	u8 raw; // Raw flags value
	struct
	{
		u8 :1;
		u8 c : 1; // Carry
		u8 z : 1; // Zero
		u8 : 3;
		u8 o : 1; // Overflow
		u8 n : 1; // Negative
	};
};