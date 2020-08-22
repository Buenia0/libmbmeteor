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
#include "gpu.h"
#include <BeeARM/beearm.h>
using namespace std;
using namespace gba;
using namespace beearm;

namespace gba
{
	class CPUInterface : public BeeARMInterface
	{
		public:
			CPUInterface(MMU &mem, Scheduler& sched, GPU& lcd);
			~CPUInterface();
			
			MMU &memory;
			Scheduler &scheduler;
			GPU& display;
			
			uint32_t lastaddr = 0;
			int clockcycles = 0;
			
			bool dump = false;
			
			int cycles(uint32_t addr, bool isu32access, bool isdata, bool isseq)
			{
				int temp = 0;
			
				bool sequential = isseq;
			
				int region = (addr >> 24);
				
				switch (region)
				{
					case 0x0:
					case 0x1: temp = 1; break;
					case 0x2: temp = (3 << isu32access); break;
					case 0x3: temp = 1; break;
					case 0x4: temp = 1; break;
					case 0x6: temp = (1 << isu32access); break;
					case 0x8:
					case 0x9:
					{
						if (sequential == true)
						{
							temp = TestBit(memory.waitcnt, 4) ? 2 : 3;
						}
						else
						{
							int nseq = ((memory.waitcnt >> 2) & 0x3);
							
							switch (nseq)
							{
								case 0: temp = 5; break;
								case 1: temp = 4; break;
								case 2: temp = 3; break;
								case 3: temp = 9; break;
							}
						}
					}
					break;
					default: temp = 1; break;
				}
				
				return temp;
			}
			
			void tick(int cycles)
			{
				clockcycles += cycles;
				scheduler.addcycles(cycles);
			}
			
			uint8_t readByte(uint32_t addr, bool isdata)
			{
				return memory.readByte(addr);
			}
			
			void writeByte(uint32_t addr, uint8_t val, bool isdata)
			{
				memory.writeByte(addr, val);
			}
			
			uint16_t readWord(uint32_t addr, bool isdata)
			{
				return memory.readWord(addr);
			}
			
			void writeWord(uint32_t addr, uint16_t val, bool isdata)
			{
				memory.writeWord(addr, val);
			}
			
			uint32_t readLong(uint32_t addr, bool isdata)
			{
				return memory.readLong(addr);
			}
			
			void writeLong(uint32_t addr, uint32_t val, bool isdata)
			{
				memory.writeLong(addr, val);
			}
			
			void softwareinterrupt(uint32_t number)
			{
				memory.softwareinterrupt(number);
			}
		
			int accesstime(uint32_t addr, bool isdata)
			{
				return 1;
			}
	};

	class LIBMBMETEOR_API CPU
	{
		public:
			CPU(MMU& mem, GPU& lcd, Scheduler& sched);
			~CPU();
			
			void init();
			void shutdown();
			
			MMU& memory;
			GPU& gpu;
			
			Scheduler& scheduler;
			
			BeeARM *arm;
			
			CPUInterface *inter = NULL;
			
			bool dump = false;
			
			int runfor(int cycles)
			{
				while (cycles > 0)
				{			
					if (memory.isdmarunning())
					{
						memory.rundma();
						continue;
					}
					
					if (memory.interruptsenabled())
					{
						if (memory.irqenable)
						{
							arm->irqexception();
						}
					}
					
					arm->executenextinstr();
													
					cycles -= inter->clockcycles;
					inter->clockcycles = 0;
				}
				
				return cycles;
			}
	};
};
