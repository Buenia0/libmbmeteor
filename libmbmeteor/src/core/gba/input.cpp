// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

#include "../../../include/libmbmeteor/gba/input.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
	Input::Input(MMU& mem) : memory(mem)
	{
		for (int i = 0x130; i < 0x132; i++)
		{
			mem.addiofuncs((0x4000000 + i), bind(&Input::readinput, this, _1), bind(&Input::writeinput, this, _1, _2));
		}
	}
	
	Input::~Input()
	{
	
	}
	
	uint8_t Input::readinput(uint32_t addr)
	{
		uint8_t temp = 0;
	
		switch ((addr & 0xFFF))
		{
			case 0x130: temp = (keyinput & 0xFF); break;
			case 0x131: temp = (keyinput >> 8); break;
		}
		
		return temp;
	}
	
	void Input::writeinput(uint32_t addr, uint8_t val)
	{
		switch ((addr & 0xFFF))
		{
			case 0x130: return; break;
			case 0x131: return; break;
		}
	}
	
	void Input::keypressed(Button button)
	{
		keyinput = BitReset(keyinput, button);
	}
	
	void Input::keyreleased(Button button)
	{
		keyinput = BitSet(keyinput, button);
	}
};
