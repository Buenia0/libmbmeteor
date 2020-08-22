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
#include <bitset>
#include "../libmbmeteor_api.h"
#include <BeeARM/beearm.h>
using namespace std;

namespace gba
{
	class LIBMBMETEOR_API Scheduler
	{
		public:
			Scheduler()
			{
			
			}
			
			~Scheduler()
			{
			
			}
			
			struct Event
			{
				function<void()> callback;
				int handle;
				int delay;
			};
			
			array<Event*, 16> events;
			
			uint16_t schedulemask = 0;
			
			void addevent(int id, int delay, function<void()> &func)
			{
				if (schedulemask & (1 << id))
				{
					cout << "Event already scheduled." << endl;
					return;
				}
				
				Event *evt = events[id];
				evt->delay = delay;
				evt->handle = id;
				evt->callback = func;
				
				schedulemask |= (1 << id);
			}
			
			void addevent(Event *evt)
			{
				if (schedulemask & (1 << evt->handle))
				{
					cout << "Event already scheduled." << endl;
					return;
				}
				
				cout << "Before: " << dec << (int)(schedulemask) << endl;
				
				events[evt->handle] = evt;
				
				schedulemask |= (1 << evt->handle);
				
				cout << "After: " << dec << (int)(schedulemask) << endl;
			}
			
			void removeevent(int id)
			{
				schedulemask &= ~(1 << id);
			}
			
			void addcycles(int cycles)
			{
				for (int i = 0; i < 16; i++)
				{
					if (schedulemask & (1 << i))
					{
						Event* evt = events[i];
						
						if (evt != NULL)
						{					
							evt->delay -= cycles;
						
							if (evt->delay <= 0)
							{
								evt->callback();
							}
						}
					}
				}
			}
	};

	using memoryreadfunc = function<uint8_t(uint32_t)>;
	using memorywritefunc = function<void(uint32_t, uint8_t)>;

	class LIBMBMETEOR_API MMU
	{
		public:
			MMU();
			~MMU();
			
			void init();
			void shutdown();
			
			array<memoryreadfunc, 0x1000> memoryreadhandlers;
			array<memorywritefunc, 0x1000> memorywritehandlers;
			
			void addiofuncs(uint32_t addr, memoryreadfunc rcb, memorywritefunc wcb)
			{
				memoryreadhandlers[(addr - 0x4000000)] = rcb;
				memorywritehandlers[(addr - 0x4000000)] = wcb;
			}
			
			vector<uint8_t> bios;
			vector<uint8_t> ewram;
			vector<uint8_t> iwram;
			vector<uint8_t> pram;
			vector<uint8_t> vram;
			vector<uint8_t> oam;
			vector<uint8_t> rom;
			
			bool loadROM(string filename);
			bool loadBIOS(string filename);
			
			BeeARM memarm;
			
			uint8_t readByte(uint32_t addr);
			void writeByte(uint32_t addr, uint8_t val);
			
			uint16_t readWord(uint32_t addr);
			void writeWord(uint32_t addr, uint16_t val);
			
			uint32_t readLong(uint32_t addr);
			void writeLong(uint32_t addr, uint32_t val);
			
			uint8_t readIO(uint32_t addr);
			void writeIO(uint32_t addr, uint8_t val);
			
			uint8_t readdma(uint32_t addr);
			void writedma(uint32_t addr, uint8_t val);
			
			uint8_t readirq(uint32_t addr);
			void writeirq(uint32_t addr, uint8_t val);
			
			void softwareinterrupt(uint32_t num);
			
			uint16_t waitcnt = 0;
			uint16_t iereg = 0;
			uint16_t ifreg = 0;
			
			bool interruptsenabled()
			{
				return ((iereg & ifreg) != 0);
			}
			
			void requestinterrupt(int id)
			{
				ifreg = BitSet(ifreg, id);
			}
			
			bool irqenable = false;
			
			// DMA transfer logic adapted from fleroviux's NanoboyAdvance
			
			int dmainc[2][4] =
			{
				{2, -2, 0, 2},
				{4, -4, 0, 4}
			};
			
			uint32_t dmasrcmask[4] = 
			{
				0x7FFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF
			};
			
			uint32_t dmadestmask[4] = 
			{
				0x7FFFFFF, 0x7FFFFFF, 0x7FFFFFF, 0xFFFFFFF
			};
			
			int dmalenmask[4] =
			{
				0x3FFF, 0x3FFF, 0x3FFF, 0xFFFF
			};
			
			int dmafrombitset[16] = 
			{
				-1, 0, 1, 0,
				2, 0, 1, 0,
				3, 0, 1, 0,
				2, 0, 1, 0
			};
		
			uint32_t latch = 0;
			
			struct DmaChannel
			{
				int dmaid = -1;
				uint32_t srcaddr;
				uint32_t destaddr;
				uint16_t count;
				uint16_t control;
				
				bool enable = false;
				bool isirq = false;
				int startmode = 0;
				bool is32bit = false;
				bool repeat = false;
				int srcinc = 0;
				int destinc = 0;
				bool isfifodma = false;
				
				bool allowread = false;
				
				struct Latch
				{
					uint32_t srcaddr;
					uint32_t destaddr;
					uint32_t length;
				};
				
				Latch latch;
			};
			
			bitset<4> runnableset;
			
			int activedmaid = -1;
			
			bool earlyexittrigger = false;
			
			bool isfifodma = false;
			
			DmaChannel channels[4];
			
			bool isdmarunning()
			{
				return runnableset.any();
			}
			
			uint8_t readdmareg(int chnum, int offset)
			{
				uint8_t temp = 0;
			
				auto &channel = channels[chnum];
				
				switch (offset)
				{
					case 8: temp = 0; break;
					case 9: temp = 0; break;
					case 10: temp = (channel.control & 0xFF); break;
					case 11: temp = (channel.control >> 8); break;
					default: temp = 0; break;
				}
				
				return temp;
			}
			
			void writedmareg(int chnum, int offset, uint8_t val)
			{
				auto &channel = channels[chnum];
				
				switch (offset)
				{
					case 0:
					case 1:
					case 2:
					case 3:
					{
						int shift = (offset << 3);
						channel.srcaddr &= ~((uint32_t)0xFF << shift);
						channel.srcaddr |= ((val << shift) & dmasrcmask[chnum]);
					}
					break;
					case 4:
					case 5:
					case 6:
					case 7:
					{
						int shift = ((offset - 4) << 3);
						channel.destaddr &= ~((uint32_t)0xFF << shift);
						channel.destaddr |= ((val << shift) & dmadestmask[chnum]);
					}
					break;
					case 8: channel.count = ((channel.count & 0xFF00) | val); break;
					case 9: channel.count = ((channel.count & 0xFF) | (val << 8)); break;
					case 10: channel.control = ((channel.control & 0xFF00) | (val & 0xE0)); break;
					case 11:
					{
						bool prevenable = channel.enable;
						
						channel.control = ((channel.control & 0xE0) | (val << 8));
						
						channel.enable = TestBit(channel.control, 15);
						channel.isirq = TestBit(channel.control, 14);
						channel.startmode = ((channel.control >> 12) & 0x3);
						channel.is32bit = TestBit(channel.control, 10);
						channel.repeat = TestBit(channel.control, 9);
						channel.srcinc = ((channel.control >> 7) & 0x3);
						channel.destinc = ((channel.control >> 5) & 0x3);
						
						channel.isfifodma = ((channel.startmode == 3) && ((chnum == 1) || (chnum == 2)));
						
						onchannelwritten(chnum, prevenable);
					}
					break;
					
				}
			}
			
			bool checksrcaddr(int chnum, int page)
			{
				return (chnum != 0 || page < 0x08 || page >= 0x0E) && page >= 0x02;
			}
			
			bool checkdestaddr(int chnum, int page)
			{
				return chnum == 3 || page < 0x08;
			}
		
			int getmemarea(int page)
			{
				if ((page >= 0x09) && (page <= 0x0D))
				{
					return 0x08;
				}
				
				if (page == 0x0F)
				{
					return 0x0E;
				}
				
				return page;
			}
			
			void onchannelwritten(int chnum, bool prevenable)
			{
				auto &channel = channels[chnum];
			
				if (channel.enable)
				{
					if (!prevenable)
					{
						int srcpage = getmemarea((channel.srcaddr >> 24));
						int destpage = getmemarea((channel.destaddr >> 24));
						
						if (checkdestaddr(chnum, destpage))
						{
							channel.latch.destaddr = channel.destaddr;
						}
						else
						{
							channel.latch.destaddr = 0;
						}
						
						if (checksrcaddr(chnum, srcpage))
						{
							channel.latch.srcaddr = channel.srcaddr;
							channel.allowread = true;
						}
						else
						{
							channel.allowread = false;
						}
						
						if (channel.is32bit == true)
						{
							channel.latch.srcaddr &= ~3;
							channel.latch.destaddr &= ~3;
						}
						else
						{
							channel.latch.srcaddr &= ~1;
							channel.latch.destaddr &= ~1;
						}
						
						channel.latch.length = (channel.count & dmalenmask[chnum]);
						
						if (channel.latch.length == 0)
						{
							channel.latch.length = (dmalenmask[chnum] + 1);
						}
						
						if (channel.startmode == 0)
						{
							startdma(chnum);
						}
					}
				}
			}
			
			void startdma(int chnum)
			{			
				if (runnableset.none())
				{
					activedmaid = chnum;
				}
				else if (chnum < activedmaid)
				{
					activedmaid = chnum;
					earlyexittrigger = true;
				}
				
				memarm.tick();
				memarm.tick();
				runnableset.set(chnum, true);
			}
			
			void rundma()
			{
				auto &channel = channels[activedmaid];
				
				if (channel.isfifodma)
				{
					cout << "FIFO DMA" << endl;
					exit(1);
				}
				else
				{
					auto srcinc = dmainc[channel.is32bit][channel.srcinc];
					auto destinc = dmainc[channel.is32bit][channel.destinc];
					
					bool first = true;
				
					#define CHECK_INTERLEAVED \
						if (earlyexittrigger == true) \
						{ \
							earlyexittrigger = false; \
							return; \
						}
						
					#define ADVANCE_REGS \
						channel.latch.srcaddr += srcinc; \
						channel.latch.destaddr += destinc; \
						channel.latch.length -= 1;
						
					if (!channel.is32bit)
					{
						if (channel.allowread)
						{
							while (channel.latch.length != 0)
							{
								CHECK_INTERLEAVED;
								
								latch = (0x10001 * memarm.readWord(channel.latch.srcaddr, true, !first));
								memarm.writeWord(channel.latch.destaddr, latch, true, !first);
								first = false;
								ADVANCE_REGS;
							}
						}
						else
						{
							while (channel.latch.length != 0)
							{
								CHECK_INTERLEAVED;
								
								memarm.writeWord(channel.latch.destaddr, latch, true, !first);
								first = false;
								ADVANCE_REGS;
							}
						}
					}
					else
					{
						if (channel.allowread)
						{
							while (channel.latch.length != 0)
							{
								// cout << dec << (int)(channel.latch.length) << endl;
								CHECK_INTERLEAVED;
								
								latch = memarm.readLong(channel.latch.srcaddr, true, !first);
								memarm.writeLong(channel.latch.destaddr, latch, true, !first);
								first = false;
								ADVANCE_REGS;
							}
						}
						else
						{
							while (channel.latch.length != 0)
							{
								CHECK_INTERLEAVED;
								
								memarm.writeLong(channel.latch.destaddr, latch, true, !first);
								first = false;
								ADVANCE_REGS;
							}
						}
					}
					
					if (channel.repeat == true)
					{
						channel.latch.length = (channel.count & dmalenmask[channel.dmaid]);
						
						if (channel.latch.length == 0)
						{
							channel.latch.length = (dmalenmask[channel.dmaid] + 1);
						}
						
						if (channel.destinc == 3)
						{
							if (checkdestaddr(channel.dmaid, (channel.destaddr >> 24)))
							{
								channel.latch.destaddr = channel.destaddr;
								if (channel.is32bit)
								{
									channel.latch.destaddr &= ~3;
								}
								else
								{
									channel.latch.destaddr &= ~1;
								}
							}
							else
							{
								channel.latch.destaddr = 0;
							}
						}
						
						runnableset.set(channel.dmaid, false);
					}
					else
					{
						channel.enable = false;
						channel.control = BitReset(channel.control, 15);
						
						runnableset.set(channel.dmaid, false);
					}
				}
				
				if (channel.isirq)
				{
					cout << "DMA IRQ" << endl;
				}
				
				activedmaid = dmafrombitset[runnableset.to_ulong()];
			}
			
			string determinegametitle(vector<uint8_t> &rom)
			{
				stringstream temp;
				
				for (int i = 0; i < 12; i++)
				{
					temp << rom[(0xA0 + i)];
				}
				
				return temp.str();
			}
			
			string determinegamecode(vector<uint8_t> &rom)
			{
				stringstream temp;
				
				for (int i = 0; i < 4; i++)
				{
					temp << rom[(0xAC + i)];
				}
				
				return temp.str();
			}
	};
};
