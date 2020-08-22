// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

#include "../../../include/libmbmeteor/gba/mmu.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
	MMU::MMU()
	{
		for (int i = 0xD4; i < 0xE0; i++)
		{
			addiofuncs((0x4000000 + i), bind(&MMU::readdma, this, _1), bind(&MMU::writedma, this, _1, _2));
		}
		
		for (int i = 0x200; i < 0x20C; i++)
		{
			addiofuncs((0x4000000 + i), bind(&MMU::readirq, this, _1), bind(&MMU::writeirq, this, _1, _2));
		}
		
		for (int i = 0; i < 4; i++)
		{
			channels[i].dmaid = i;
		}
	}
	
	MMU::~MMU()
	{

	}
	
	void MMU::init()
	{
		ewram.resize(0x40000, 0);
		iwram.resize(0x8000, 0);
		pram.resize(0x400, 0);
		vram.resize(0x18000, 0);
		oam.resize(0x400, 0);
		cout << "MMU::Initialized" << endl;
	}
	
	void MMU::shutdown()
	{
		ewram.clear();
		iwram.clear();
		pram.clear();
		vram.clear();
		oam.clear();
		rom.clear();
		cout << "MMU::Shutting down..." << endl;
	}
	
	uint8_t MMU::readByte(uint32_t addr)
	{
		uint8_t temp = 0;
	
		int region = (addr >> 24);
		
		switch (region)
		{
			case 0x0:
			{
				if (addr < 0x4000)
				{
					temp = bios[addr];
				}
				else
				{
					// TODO: Open-bus behavior
					temp = 0;
				}
			}
			break;
			case 0x2: temp = ewram[(addr & 0x3FFFF)]; break;
			case 0x3: temp = iwram[(addr & 0x7FFF)]; break;
			case 0x4: temp = readIO(addr); break;
			case 0x5: temp = pram[(addr & 0x3FF)]; break;
			case 0x6:
			{
				uint32_t tempaddr = (addr & 0x1FFFF);
				
				if (tempaddr >= 0x18000)
				{
					tempaddr -= 0x8000;
				}
				
				temp = vram[tempaddr];
			}
			break;
			case 0x7: temp = oam[(addr & 0x3FF)]; break;
			case 0x8:
			case 0x9: temp = rom[(addr - 0x8000000)]; break;
			case 0xD: temp = 0; break; // TODO: EEPROM saving
			case 0xE: temp = 0; break; // TODO: SRAM saving
			default: cout << "Unrecognized read from region of " << hex << (int)(region) << endl; memarm.printregs(); exit(1); temp = 0; break;
		}
		
		return temp;
	}
	
	void MMU::writeByte(uint32_t addr, uint8_t val)
	{
		int region = (addr >> 24);
		
		switch (region)
		{
			case 0x0: return; break;
			case 0x2: ewram[(addr & 0x3FFFF)] = val; break;
			case 0x3: iwram[(addr & 0x7FFF)] = val; break;
			case 0x4: writeIO(addr, val); break;
			case 0x5:
			case 0x6:
			case 0x7:
			{
				// PRAM VRAM, and OAM are not accesible with 8-bit write widths
				return;
			}
			break;
			case 0x8:
			case 0x9: return; break;
			case 0xD: return; break;
			case 0xE: return; break;
			default: cout << "Unrecognized write to region of " << hex << (int)(region) << endl; memarm.printregs(); exit(1); break;
		}
	}
	
	uint16_t MMU::readWord(uint32_t addr)
	{
		uint16_t temp = 0;
	
		int region = (addr >> 24);
		
		switch (region)
		{
			case 0x0:
			{
				if (addr < 0x4000)
				{
					temp = *(uint16_t*)&bios[addr];
				}
				else
				{
					// TODO: Open-bus behavior
					temp = 0;
				}
			}
			break;
			case 0x2: temp = *(uint16_t*)&ewram[(addr & 0x3FFFF)]; break;
			case 0x3: temp = *(uint16_t*)&iwram[(addr & 0x7FFF)]; break;
			case 0x4:
			{
				temp = ((readIO(addr + 1) << 8) | (readIO(addr)));
			}
			break;
			case 0x5: temp = *(uint16_t*)&pram[(addr & 0x3FF)]; break;
			case 0x6:
			{
				uint32_t tempaddr = (addr & 0x1FFFF);
				
				if (tempaddr >= 0x18000)
				{
					tempaddr -= 0x8000;
				}
				
				temp = *(uint16_t*)&vram[tempaddr];
			}
			break;
			case 0x7: temp = *(uint16_t*)&oam[(addr & 0x3FF)]; break;
			case 0x8:
			case 0x9: temp = *(uint16_t*)&rom[(addr - 0x8000000)]; break;
			case 0xD: temp = 0; break; // TODO: EEPROM saving
			default: cout << "Unrecognized read from region of " << hex << (int)(region) << endl; memarm.printregs(); exit(1); temp = 0; break;
		}
		
		return temp;
	}
	
	void MMU::writeWord(uint32_t addr, uint16_t val)
	{
		int region = (addr >> 24);
		
		switch (region)
		{
			case 0x0: return; break;
			case 0x2: *(uint16_t*)&ewram[(addr & 0x3FFFF)] = val; break;
			case 0x3: *(uint16_t*)&iwram[(addr & 0x7FFF)] = val; break;
			case 0x4:
			{
				writeIO(addr, (val & 0xFF));
				writeIO((addr + 1), (val >> 8));
			}
			break;
			case 0x5: *(uint16_t*)&pram[(addr & 0x3FF)] = val; break;
			case 0x6:
			{
				uint32_t tempaddr = (addr & 0x1FFFF);
				
				if (tempaddr >= 0x18000)
				{
					tempaddr -= 0x8000;
				}
				
				*(uint16_t*)&vram[tempaddr] = val;
			}
			break;
			case 0x7: *(uint16_t*)&oam[(addr & 0x3FF)] = val; break;
			case 0x8:
			case 0x9: return; break;
			case 0xD: return; break; // TODO: EEPROM saving
			default: cout << "Unrecognized write to region of " << hex << (int)(region) << endl; memarm.printregs(); exit(1); break;
		}
	}
	
	uint32_t MMU::readLong(uint32_t addr)
	{
		uint32_t temp = 0;
	
		int region = (addr >> 24);
		
		switch (region)
		{
			case 0x0:
			{
				if (addr < 0x4000)
				{
					temp = *(uint32_t*)&bios[addr];
				}
				else
				{
					// TODO: Open-bus behavior
					temp = 0;
				}
			}
			break;
			case 0x1:
			{
			    memarm.printregs();
			    exit(1);
			}
			break;
			case 0x2: temp = *(uint32_t*)&ewram[(addr & 0x3FFFF)]; break;
			case 0x3: temp = *(uint32_t*)&iwram[(addr & 0x7FFF)]; break;
			case 0x4:
			{
				uint32_t iotemp = 0;
				
				for (int i = 0; i < 4; i++)
				{
					iotemp |= (readIO((addr + i)) << (i << 3));
				}
				
				temp = iotemp;
			}
			break;
			case 0x5: temp = *(uint32_t*)&pram[(addr & 0x3FF)]; break;
			case 0x6:
			{
				uint32_t tempaddr = (addr & 0x1FFFF);
				
				if (tempaddr >= 0x18000)
				{
					tempaddr -= 0x8000;
				}
				
				temp = *(uint32_t*)&vram[tempaddr];
			}
			break;
			case 0x7: temp = *(uint32_t*)&oam[(addr & 0x3FF)]; break;
			case 0x8:
			case 0x9: temp = *(uint32_t*)&rom[(addr - 0x8000000)]; break;
			default: cout << "Unrecognized read from region of " << hex << (int)(region) << endl; memarm.printregs(); exit(1); temp = 0; break;
		}
		
		return temp;
	}
	
	void MMU::writeLong(uint32_t addr, uint32_t val)
	{
		int region = (addr >> 24);
		
		switch (region)
		{
			case 0x0: return; break;
			case 0x2: *(uint32_t*)&ewram[(addr & 0x3FFFF)] = val; break;
			case 0x3: *(uint32_t*)&iwram[(addr & 0x7FFF)] = val; break;
			case 0x4:
			{
				for (int i = 0; i < 4; i++)
				{
					writeIO((addr + i), ((val >> (i << 3)) & 0xFF));
				}
			}
			break;
			case 0x5: *(uint32_t*)&pram[(addr & 0x3FF)] = val; break;
			case 0x6:
			{
				uint32_t tempaddr = (addr & 0x1FFFF);
				
				if (tempaddr >= 0x18000)
				{
					tempaddr -= 0x8000;
				}
				
				*(uint32_t*)&vram[tempaddr] = val;
			}
			break;
			case 0x7: *(uint32_t*)&oam[(addr & 0x3FF)] = val; break;
			case 0x8:
			case 0x9: return; break;
			default: cout << "Unrecognized write to region of " << hex << (int)(region) << endl; memarm.printregs(); exit(1); break;
		}
	}
	
	uint8_t MMU::readIO(uint32_t addr)
	{
		uint8_t temp = 0;
		
		if (memoryreadhandlers[(addr - 0x4000000)])
		{
			temp = memoryreadhandlers[(addr - 0x4000000)](addr);
		}
		else
		{
			temp = 0xFF;
		}
		
		return temp;
	}
	
	void MMU::writeIO(uint32_t addr, uint8_t val)
	{
		if (memorywritehandlers[(addr - 0x4000000)])
		{
			memorywritehandlers[(addr - 0x4000000)](addr, val);
		}
	}
	
	uint8_t MMU::readdma(uint32_t addr)
	{
		uint8_t temp = 0;
		
		switch ((addr & 0xFF))
		{
			case 0xDC: temp = readdmareg(3, 8); break;
			case 0xDD: temp = readdmareg(3, 9); break;
			case 0xDE: temp = readdmareg(3, 10); break;
			case 0xDF: temp = readdmareg(3, 11); break;
		}
		
		return temp;
	}
	
	void MMU::writedma(uint32_t addr, uint8_t val)
	{
		switch ((addr & 0xFF))
		{
			case 0xD4: writedmareg(3, 0, val); break;
			case 0xD5: writedmareg(3, 1, val); break;
			case 0xD6: writedmareg(3, 2, val); break;
			case 0xD7: writedmareg(3, 3, val); break;
			case 0xD8: writedmareg(3, 4, val); break;
			case 0xD9: writedmareg(3, 5, val); break;
			case 0xDA: writedmareg(3, 6, val); break;
			case 0xDB: writedmareg(3, 7, val); break;
			case 0xDC: writedmareg(3, 8, val); break;
			case 0xDD: writedmareg(3, 9, val); break;
			case 0xDE: writedmareg(3, 10, val); break;
			case 0xDF: writedmareg(3, 11, val); break;
		}
		
		return;
	}
	
	uint8_t MMU::readirq(uint32_t addr)
	{
		uint8_t temp = 0;
	
		switch ((addr & 0xFFF))
		{
			case 0x200: temp = (iereg & 0xFF); break;
			case 0x201: temp = (iereg >> 8); break;
			case 0x202: temp = (ifreg & 0xFF); break;
			case 0x203: temp = (ifreg >> 8); break;
			case 0x204: temp = (waitcnt & 0xFF); break;
			case 0x205: temp = (waitcnt >> 8); break;
			case 0x208: temp = irqenable; break;
			case 0x209: temp = 0; break;
			case 0x20A: temp = 0; break;
			case 0x20B: temp = 0; break;
			default: cout << "Unrecognized read of " << hex << (int)(addr) << endl; exit(1); temp = 0; break;
		}
		
		return temp;
	}
	
	void MMU::writeirq(uint32_t addr, uint8_t val)
	{
		switch ((addr & 0xFFF))
		{
			case 0x200: iereg = ((iereg & 0x3F00) | val); break;
			case 0x201: iereg = ((iereg & 0xFF) | ((val & 0x3F) << 8)); break;
			case 0x202: ifreg &= ~val; break;
			case 0x203: ifreg &= ~(val << 8); break;
			case 0x204: waitcnt = ((waitcnt & 0x7F00) | val); break;
			case 0x205: waitcnt = ((waitcnt & 0xFF) | ((val & 0x7F) << 8)); break;
			case 0x206: return; break;
			case 0x207: return; break;
			case 0x208: irqenable = TestBit(val, 0); break;
			case 0x209: return; break;
			case 0x20A: return; break;
			case 0x20B: return; break;
			default: cout << "Unrecognized write of " << hex << (int)(addr) << endl; exit(1); break;
		}
	}
	
	void MMU::softwareinterrupt(uint32_t number)
	{
		memarm.swiexception();
	}
	
	bool MMU::loadROM(string filename)
	{
		cout << "MMU::Loading ROM..." << endl;
		ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);
		
		if (file.is_open())
		{
			streampos size = file.tellg();
			rom.resize(0x2000000, 0);
			file.seekg(0, ios::beg);
			file.read((char*)rom.data(), size);
			
			cout << "Title: " << determinegametitle(rom) << endl;
			cout << "Game Code: " << determinegamecode(rom) << endl;
			cout << "MMU::" << filename << " succesfully loaded." << endl;
			file.close();
			return true;
		}
		else
		{
			cout << "MMU::Error - " << filename << " could not be opened." << endl;
			return false;
		}
	}
	
	bool MMU::loadBIOS(string filename)
	{
		cout << "MMU::Loading BIOS..." << endl;
		ifstream file(filename.c_str(), ios::in | ios::binary | ios::ate);
		
		if (file.is_open())
		{
			streampos size = file.tellg();
			bios.resize(0x4000, 0);
			file.seekg(0, ios::beg);
			file.read((char*)bios.data(), size);
			
			cout << "MMU::BIOS succesfully loaded." << endl;
			file.close();
			return true;
		}
		else
		{
			cout << "MMU::Error - BIOS could not be opened." << endl;
			return false;
		}
	}
};
