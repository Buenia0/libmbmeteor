// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

#pragma once
#include <cstdint>
#include <functional>
#include <iostream>
#include <sstream>
#include <fstream>
#include "../libmbmeteor_api.h"
#include "mmu.h"
using namespace std;
using namespace gba;

namespace gba
{
	enum Button : int
	{
		A = 0,
		B = 1,
		Select = 2,
		Start = 3,
		Right = 4,
		Left = 5,
		Up = 6,
		Down = 7,
		R = 8,
		L = 9
	};

	class LIBMBMETEOR_API Input
	{
		public:
			Input(MMU& mem);
			~Input();
			
			MMU& memory;
			
			uint8_t readinput(uint32_t addr);
			void writeinput(uint32_t addr, uint8_t val);
			
			uint16_t keyinput = 0x3FF;
			
			void keypressed(Button button);
			void keyreleased(Button button);
	};
};
