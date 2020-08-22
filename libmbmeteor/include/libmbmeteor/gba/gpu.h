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
	struct RGB
	{
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
	
	using gpufunc = function<void()>;

	class LIBMBMETEOR_API GPU
	{
		public:
			GPU(MMU& mem, Scheduler& sched);
			~GPU();
			
			void init();
			void shutdown();
			
			enum Phase : int
			{
				Scanline = 0,
				HBlankSearch = 1,
				HBlank = 2,
				VBlankScanline = 3,
				VBlankHBlank = 4
			};
			
			gpufunc drawpixels;
			
			void setpixelcallback(gpufunc cb)
			{
				drawpixels = cb;
			}
			
			void setnextevent(Phase gpuphase);
			
			Phase phase;
			
			int waitcycles[5] = {960, 46, 226, 1006, 226};
			
			uint16_t dispcnt = 0;
			uint16_t dispstat = 0;
			uint16_t vcount = 0;
			uint16_t bgcnt[4] = {{0}};
			uint16_t bghoffs[4] = {{0}};
			uint16_t bgvoffs[4] = {{0}};
			
			uint16_t bldcnt = 0;
			int bldalphaeva = 0;
			int bldalphaevb = 0;
			int bldy = 0;
			
			void vblank(bool val)
			{
				dispstat = BitChange(dispstat, 0, val);
			}
			
			void hblank(bool val)
			{
				dispstat = BitChange(dispstat, 1, val);
			}
			
			MMU& memory;
			
			Scheduler& scheduler;
			
			Scheduler::Event evt {[this] { this->tick(); }, 0, 0};
			
			uint8_t readgpu(uint32_t addr);
			void writegpu(uint32_t addr, uint8_t val);
			
			void tick();
			
			void renderscanline();
			
			void renderbgmode0();
			void renderbgmode1();
			void renderbgmode3();
			void renderbgmode4();

			
			void renderbgtext(int num);
			void renderbgbitmap(int num, int modenum);
			void renderbgtrans(int num);
			void composescanline(int start, int end);
			
			uint16_t alphablend(uint16_t firstcol, uint16_t secondcol); 
			uint16_t brightnessblend(bool increase, uint16_t color);
			
			void clearscreen()
			{
				for (int i = 0; i < (240 * 160); i++)
				{
					framebuffer[i] = {0, 0, 0};
				}
			}
			
			array<RGB, (240 * 160)> framebuffer;
			array<array<uint16_t, 240>, 4> bgbuffer;
			array<array<int, 240>, 4> bginfobuffer;
			
			uint16_t readvram16(uint32_t addr)
			{
				return ((memory.vram[(addr + 1)] << 8) + (memory.vram[addr]));
			}
			
			uint16_t readpram16(int palette, int index)
			{
				int cell = ((palette << 5) + (index << 1));
				return ((memory.pram[(cell + 1)] << 8) | (memory.pram[cell]));
			}
	};
};
