// This file is part of libmbmeteor.
// Copyright (C) 2019 Buenia.
//
// libmbmeteor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libmbmeteor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libmbmeteor.  If not, see <https://www.gnu.org/licenses/>.

#ifndef LIBMBMETEOR_MMU
#define LIBMBMETEOR_MMU

#include "../libmbmeteor_api.h"
#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <bitset>
#include <functional>
#include <BeeARM/beearm.h>
#include <BeeARM/beearm_tables.h>
using namespace std;
using namespace beearm;

namespace gba
{
	using memoryreadfunc = function<uint8_t(uint32_t)>;
	using memorywritefunc = function<void(uint32_t, uint8_t)>;
	
    class LIBMBMETEOR_API MMU
    {
	public:
	    MMU();
	    ~MMU();

		void init();
		void shutdown();

		vector<uint8_t> bios;
		vector<uint8_t> wram256;
		vector<uint8_t> wram32;
		vector<uint8_t> pram;
		vector<uint8_t> vram;
		vector<uint8_t> oam;
	    vector<uint8_t> gamerom;
	    vector<uint8_t> gamesram;
	    vector<uint8_t> gameeeprom;
	    vector<uint8_t> gameflash;

	    array<uint8_t, 0x1000000> memorymap;

	    // GPIO Stuff
	    bool rtcenable = false;
	    bool isgpioreadable = false;
	    int gpiodata = 0;
	    int gpiodirection = 0;
	    int rtcstate = 0;
	    int rtccounter = 0;
	    uint8_t rtcbyte = 0;
	    array<uint8_t, 7> rtcdata = {{0}};
	    int rtclength = 0;
	    int rtcserial = 0;
	    int rtcbitcounter = 0;
	    int rtcwritetype = 3;
	    void processrtc();

	    struct RTCDateTime
	    {
		uint8_t control = 0x40;

		int year = 0;
		int month = 1;
		int day = 1;
		int week = 0;
		int hours = 0;
		int minutes = 0;
		int seconds = 0;

		void reset()
		{
		    year = 0;
		    month = 1;
		    day = 1;
		    week = 0;
		    hours = 0;
		    minutes = 0;
		    seconds = 0;
		}

		void writeregisters(int regtype, array<uint8_t, 7> &val)
		{
		    switch ((regtype & 0x3))
		    {
			case 0:
			{
			    writecontrol(val);
			}
			break;
			case 1:
			{
			    return;
			}
			break;
			case 2:
			{
			    return;
			}
			break;
			case 3: return; break;
		    }
		}

	 	uint16_t tobcd(uint16_t temp)
		{
		    return (((temp / 10) << 4) | (temp % 10));
		}

		void updatertctime()
		{
		    time_t currenttime = time(NULL);

		    tm *rtctime = localtime(&currenttime);

		    year = tobcd((rtctime->tm_year - 100));
		    month = tobcd((rtctime->tm_mon + 1));
		    day = tobcd(rtctime->tm_mday);
		    week = tobcd(rtctime->tm_wday);
		    hours = tobcd((rtctime->tm_hour % (12 << TestBit(control, 6))));
		    minutes = tobcd((rtctime->tm_min % 60));
		    int realsecs = (rtctime->tm_sec > 59) ? 59 : rtctime->tm_sec;
		    seconds = tobcd((realsecs % 60));	    
		}

		array<uint8_t, 7> readcontrol()
		{
		    array<uint8_t, 7> temp = {{0}};

		    temp[0] = control;

		    return temp;
		}

		void writecontrol(array<uint8_t, 7> &val)
		{
		    control = val[0];
		}

		array<uint8_t, 7> readdatetime()
		{
		    updatertctime();
		    array<uint8_t, 7> temp = {{0}};
		    temp[0] = year;
		    temp[1] = month;
		    temp[2] = day;
		    temp[3] = week;
		    temp[4] = hours;
		    temp[5] = minutes;
		    temp[6] = seconds;

		    return temp;
		}

		array<uint8_t, 7> readtime()
		{
		    updatertctime();
		    array<uint8_t, 7> temp = {{0}};
		    temp[0] = hours;
		    temp[1] = minutes;
		    temp[2] = seconds;

		    return temp;
		}
	    };

	    RTCDateTime rtcdtregs;

	    bool solarenable = false;
	    int solarcounter = 0;
	    int solarvalue = 0xE8;
	    void processsolar();
	    void increasesolar();
	    void decreasesolar();

	    string getgamecodef3(vector<uint8_t> &rom)
	    {
		stringstream ss;

		for (int i = 0; i < 3; i++)
		{
		    ss << rom[(0xAC + i)];
		}

		return ss.str();
	    }

	    bool isrtcsupported(vector<uint8_t> &rom)
	    {
		bool temp = false;
		string gamecode = getgamecodef3(rom);

		switch (rom[0xAC])
		{
		    case 0x41: temp = ((gamecode == "AXV") || (gamecode == "AXP")); break;
		    case 0x42: temp = ((gamecode == "BPE") || (gamecode == "BR4") || (gamecode == "BKA")); break;
		    case 0x55: temp = true; break;
		    default: temp = false; break;
		}

		return temp;
	    }

	    bool issolarsupported(vector<uint8_t> &rom)
	    {
		return (rom[0xAC] == 0x55);
	    }

	    uint32_t getbcd(uint32_t val)
	    {
		return (((val / 10) << 4) | (val % 10));
	    }

	    bool iseepromsave = false;
		
		BeeARM memarm; // For "open-bus" behavior
		
		array<memoryreadfunc, 0xFFF> memoryreadhandlers;
		array<memorywritefunc, 0xFFF> memorywritehandlers;

		bool dump = false;

		int waitstate = 0;
		
		void addmemoryreadhandler(uint32_t addr, memoryreadfunc cb)
		{
			memoryreadhandlers.at((addr - 0x4000000)) = cb;
		}
		
		void addmemorywritehandler(uint32_t addr, memorywritefunc cb)
		{
			memorywritehandlers.at((addr - 0x4000000)) = cb;
		}
		
		uint8_t readtemp(uint32_t addr)
		{
		    // cout << "Unrecognized read from " << hex << (int)(addr) << endl;
		    return 0x00;
		}
		
		void writetemp(uint32_t addr, uint8_t val)
		{
		    // cout << "Unrecognized write to " << hex << (int)(addr) << endl;
		    return;
		}
		
		uint32_t lastbiosvalue = 0;

		bool iseeprom()
		{
		    return (carttype == CartridgeType::CartEEPROM);
		}

		bool issram()
		{
		    return (carttype == CartridgeType::CartSRAM);
		}

		bool isflash128k()
		{
		    return (carttype == CartridgeType::CartFlash128K);
		}

		bool isflash64k()
		{
		    return (carttype == CartridgeType::CartFlash64K);
		}

	    uint8_t readByte(uint32_t addr);
	    void writeByte(uint32_t addr, uint8_t val);
	    uint16_t readWord(uint32_t addr);
	    void writeWord(uint32_t addr, uint16_t val);
	    uint32_t readLong(uint32_t addr);
	    void writeLong(uint32_t addr, uint32_t val);

	    uint8_t readdma(uint32_t addr);
	    void writedma(uint32_t addr, uint8_t val);

	    uint8_t readinterrupts(uint32_t addr);
	    void writeinterrupts(uint32_t addr, uint8_t val);

	    uint16_t readeeprom(uint32_t addr);
	    void writeeeprom(uint16_t val);
	    
	    int eepromstate = 1;
	    int eeprombuffer = 0;
	    int eepromlength = 0;
	    int eepromcmd = 0;
	    int eeprombitsize = 6;
	    int eepromreadaddr = 0;
	    int eepromwriteaddr = 0;
	    int eeprombitmask = 0x3F;
	    int eeprombitsread = 0;
	    int eeprombyteswritten = 0;
	    bool eepromlock = false;

	    uint8_t readflash128k(uint32_t addr);
	    void writeflash128k(uint32_t addr, uint8_t val);

	    uint8_t readflash64k(uint32_t addr);
	    void writeflash64k(uint32_t addr, uint8_t val);

	    int flashstate = 0;
	    bool flashgrabid = false;
	    bool flashswitch = false;
	    bool flashwrite = false;
	    int flash128kbank = 0;
	    uint16_t flash128id = 0x09C2; // Macronix 128K Flash ROM ID
	    uint16_t flash64id = 0x1B32; // Panasonic 64K Flash ROM ID

	    void softwareinterrupt();

	    uint16_t iereg = 0;
	    uint16_t ifreg = 0;
	    bool interruptmasterenable = false;

	    uint16_t waitcnt = 0;

	    void setinterruptmirror(int num, bool val)
	    {
		writeLong(0x3007FF8, BitChange(readLong(0x3007FF8), num, val));
	    }

	    void setinterrupt(int num)
	    {
		ifreg = BitSet(ifreg, num);
	    }

	    void clearinterrupt(int num)
	    {
		ifreg = BitReset(ifreg, num);
	    }

	    bool isinterruptsenabled()
	    {
		return (iereg & ifreg);
	    }

	    enum DmaState : int
	    {
		Inactive = -1,
		Starting = 0,
		Paused = 1,
		Active = 2,
	    };

	    uint32_t dma0src = 0;
	    uint32_t dma0dst = 0;
	    uint16_t dma0length = 0;
	    uint16_t dma0control = 0;
	    int dma0unitstocopy = 0;
	    uint32_t dma0oldsrc = 0;
	    uint32_t dma0olddst = 0;
	    int dma0oldlength = 0;

	    uint32_t dma1src = 0;
	    uint32_t dma1fifosrc = 0;
	    uint32_t dma1dst = 0;
	    uint16_t dma1length = 0;
	    uint16_t dma1control = 0;
	    int dma1unitstocopy = 0;
	    bool dma1fifo = false;
	    bool dma1req = false;

	    uint32_t dma2src = 0;
	    uint32_t dma2fifosrc = 0;
	    uint32_t dma2dst = 0;
	    uint16_t dma2length = 0;
	    uint16_t dma2control = 0;
	    int dma2unitstocopy = 0;
	    bool dma2fifo = false;
	    bool dma2req = false;

	    uint32_t dma3src = 0;
	    uint32_t dma3dst = 0;
	    uint16_t dma3length = 0;
	    uint16_t dma3control = 0;
	    int dma3unitstocopy = 0;

	    DmaState dma0state = DmaState::Inactive;
	    DmaState dma1state = DmaState::Inactive;
	    DmaState dma2state = DmaState::Inactive;
	    DmaState dma3state = DmaState::Inactive;
	    
	    uint32_t dmavalue = 0;
	    uint32_t dmapc = 0;

	    void decdmaunits(int num)
	    {
		switch ((num & 0x3))
		{
		    case 0: dma0unitstocopy -= 1; break;
		    case 1: dma1unitstocopy -= 1; break;
		    case 2: dma2unitstocopy -= 1; break;
		    case 3: dma3unitstocopy -= 1; break;
		}
	    }

	    uint32_t getdmasrc(int num)
	    {
		uint32_t temp;

		switch ((num & 0x3))
		{
		    case 0: temp = dma0src; break;
		    case 1: temp = dma1src; break;
		    case 2: temp = dma2src; break;
		    case 3: temp = dma3src; break;
		}

		return temp;
	    }

	    uint32_t getdmadst(int num)
	    {
		uint32_t temp;

		switch ((num & 0x3))
		{
		    case 0: temp = dma0dst; break;
		    case 1: temp = dma1dst; break;
		    case 2: temp = dma2dst; break;
		    case 3: temp = dma3dst; break;
		}

		return temp;
	    }

	    uint32_t getdmacontrol(int num)
	    {
		uint32_t temp;

		switch ((num & 0x3))
		{
		    case 0: temp = dma0control; break;
		    case 1: temp = dma1control; break;
		    case 2: temp = dma2control; break;
		    case 3: temp = dma3control; break;
		}

		return temp;
	    }

	    void disabledma(int num)
	    {
		switch ((num & 0x3))
		{
		    case 0: dma0control = BitReset(dma0control, 15); break;
		    case 1: dma1control = BitReset(dma1control, 15); break;
		    case 2: dma2control = BitReset(dma2control, 15); break;
		    case 3: dma3control = BitReset(dma3control, 15); break;
		}
	    }

	    DmaState getdmastate(int num)
	    {
		DmaState temp;

		switch ((num & 0x3))
		{
		    case 0: temp = dma0state; break;
		    case 1: temp = dma1state; break;
		    case 2: temp = dma2state; break;
		    case 3: temp = dma3state; break;
		}

		return temp;
	    }

	    void setdmastate(int num, DmaState val)
	    {
		switch ((num & 0x3))
		{
		    case 0: dma0state = val; break;
		    case 1: dma1state = val; break;
		    case 2: dma2state = val; break;
		    case 3: dma3state = val; break;
		}
	    }

	    void aligndma(int num)
	    {
		switch ((num & 0x3))
		{
		    case 0:
		    {
			dma0src &= ~(TestBit(dma0control, 10) ? 3 : 1);
			dma0dst &= ~(TestBit(dma0control, 10) ? 3 : 1);
		    }
		    break;
		    case 1:
		    {
			dma1src &= ~(TestBit(dma1control, 10) ? 3 : 1);
			dma1dst &= ~(TestBit(dma1control, 10) ? 3 : 1);
		    }
		    break;
		    case 2:
		    {
			dma2src &= ~(TestBit(dma2control, 10) ? 3 : 1);
			dma2dst &= ~(TestBit(dma2control, 10) ? 3 : 1);
		    }
		    break;
		    case 3:
		    {
			dma3src &= ~(TestBit(dma3control, 10) ? 3 : 1);
			dma3dst &= ~(TestBit(dma3control, 10) ? 3 : 1);
		    }
		    break;
		}
	    }

	    int getdmaunits(int num)
	    {
		int temp;

		switch ((num & 0x3))
		{
		    case 0: temp = dma0unitstocopy; break;
		    case 1: temp = dma1unitstocopy; break;
		    case 2: temp = dma2unitstocopy; break;
		    case 3: temp = dma3unitstocopy; break;
		}

		return temp;
	    }

	    void incdma(int num, int control, bool dst)
	    {
		int increment = (dmainc(control, dst) << (1 + TestBit(getdmacontrol(num), 10)));

		switch ((num & 0x3))
		{
		    case 0:
		    {
			if (dst)
			{
			    dma0dst += increment;
			}
			else
			{
			    dma0src += increment;
			}
		    }
		    break;
		    case 1:
		    {
			if (dst)
			{
			    dma1dst += increment;
			}
			else
			{
			    dma1src += increment;
			}
		    }
		    break;
		    case 2:
		    {
			if (dst)
			{
			    dma2dst += increment;
			}
			else
			{
			    dma2src += increment;
			}
		    }
		    break;
		    case 3:
		    {
			if (dst)
			{
			    dma3dst += increment;
			}
			else
			{
			    dma3src += increment;
			}
		    }
		    break;
		}
	    }

	    int dmainc(int num, bool dst)
	    {
		int temp = 0;
	
		switch ((num & 0x3))
		{
		    case 0: temp = 1; break;
		    case 1: temp = -1; break;
		    case 2: temp = 0; break;
		    case 3:
		    {
			if (dst)
			{
			    temp = 1;
			}
			else
			{
			    cout << "Prohibited" << endl;
			    exit(1);
			}
		    }
		    break;
		}

		return temp;
	    }

	    void resetdmadst(int num, uint32_t val)
	    {
		switch ((num & 0x3))
		{
		    case 0: dma0dst -= val; break;
		    case 1: dma1dst -= val; break;
		    case 2: dma2dst -= val; break;
		    case 3: dma3dst -= val; break;
		}
	    }

	    void resetdmalength(int num)
	    {
		switch ((num & 0x3))
		{
		    case 0: dma0unitstocopy = (dma0length == 0) ? 0x4000 : dma0length; break;
		    case 1: dma1unitstocopy = (dma1length == 0) ? 0x4000 : dma1length; break;
		    case 2: dma2unitstocopy = (dma2length == 0) ? 0x4000 : dma2length; break;
		    case 3: dma3unitstocopy = (dma3length == 0) ? 0x10000 : dma3length; break;
		}		
	    }

	    uint32_t getdmalength(int num)
	    {
		uint32_t temp;

		switch ((num & 0x3))
		{
		    case 0: temp = (dma0length == 0) ? 0x4000 : dma0length; break;
		    case 1: temp = (dma1length == 0) ? 0x4000 : dma1length; break;
		    case 2: temp = (dma2length == 0) ? 0x4000 : dma2length; break;
		    case 3: temp = (dma3length == 0) ? 0x10000 : dma3length; break;
		}

		return temp;
	    }

	    bool isvblank()
	    {
		uint16_t dispstat = readWord(0x4000004);

		return TestBit(dispstat, 0);
	    }

	    bool ishblank()
	    {
		uint16_t dispstat = readWord(0x4000004);

		return TestBit(dispstat, 1);
	    }

	    void signalhblank()
	    {
		for (int i = 0; i < 4; i++)
		{
		    if ((i == 1) && (dma1fifo == true))
		    {
			continue;
		    }

		    if ((i == 2) && (dma2fifo == true))
		    {
			continue;
		    }

		    if ((getdmastate(i) == DmaState::Paused))
		    {
			resetdmalength(i);
			setdmastate(i, DmaState::Starting);
		    }
		}
	    }

	    void signalvblank()
	    {
		for (int i = 0; i < 4; i++)
		{
		    if ((i == 1) && (dma1fifo == true))
		    {
			continue;
		    }

		    if ((i == 2) && (dma2fifo == true))
		    {
			continue;
		    }

		    if ((getdmastate(i) == DmaState::Paused))
		    {
			resetdmalength(i);
			setdmastate(i, DmaState::Starting);
		    }
		}
	    }

	    void signalfifo(int num)
	    {
		if ((num == 1) && (dma1fifo == true) && (dma1req == true))
		{
		    if (getdmastate(num) == DmaState::Paused)
		    {
			dma1unitstocopy = 4;
			setdmastate(num, DmaState::Starting);
		    }
		}

		if ((num == 2) && (dma2fifo == true) && (dma2req == true))
		{
		    if (getdmastate(num) == DmaState::Paused)
		    {
			dma2unitstocopy = 4;
			setdmastate(num, DmaState::Starting);
		    }
		}
	    }
	    
	    void setupdmafifo()
	    {
	    	if ((dma1dst & 0xF0) != 0xA0)
	    	{
	    	    dma1dst = TestBit(dma1dst, 2) ? 0x40000A4 : 0x40000A0;
	    	}
	    	
	    	if ((dma2dst & 0xF0) != 0xA0)
	    	{
	    	    dma2dst = TestBit(dma2dst, 2) ? 0x40000A4 : 0x40000A0;
	    	}
	    }

	    void setupdma(int num)
	    {
		aligndma(num);
		resetdmalength(num);
		
		if ((num == 3) && (getdmadst(num) >> 24) == 0xD)
		{
		    cout << "EEPROM" << endl;
		    if (((getdmaunits(num) == 17) || (getdmaunits(num) == 81)) && !eepromlock)
		    {
		    	eeprombitsize = 14;
		    	eeprombitmask = 0x3FF;
		    	gameeeprom.clear();
		    	gameeeprom.resize(0x2000, 0);
			eepromlock = true;
		    }
		}
		
		dmapc = memarm.getreg(15);

		int starttiming = ((getdmacontrol(num) >> 12) & 0x3);

		switch (starttiming)
		{
		    case 0: setdmastate(num, DmaState::Starting); break;
		    case 1: setdmastate(num, (isvblank() ? DmaState::Starting : DmaState::Paused)); break;
		    case 2: setdmastate(num, (ishblank() ? DmaState::Starting : DmaState::Paused)); break;
		    case 3:
		    {
			switch (num)
			{
			    case 0: cout << "Prohibited" << endl; exit(1); break;
			    case 1:
			    {
				setdmastate(1, DmaState::Paused);
				dma1unitstocopy = 4;
				dma1src = dma1fifosrc;	
				dma1fifo = true;
				setupdmafifo();
			    }
			    break;
			    case 2:
			    {
				setdmastate(2, DmaState::Paused);
				dma2unitstocopy = 4;
				dma2src = dma2fifosrc;
				dma2fifo = true;
				setupdmafifo();
			    }
			    break;
			    case 3: cout << "DMA3 Video Capture" << endl; break;
			}
		    }
		    break;
		}
	    }

	    void enddma(int num)
	    {
		setdmastate(num, DmaState::Inactive);
		disabledma(num);

		if (TestBit(getdmacontrol(num), 14))
		{
		    cout << "DMA IRQ" << endl;
		    // setinterrupt((8 + num));
		}
	    }

	    void updatedma(int num)
	    {
		if (getdmastate(num) == DmaState::Starting)
		{
		    setdmastate(num, DmaState::Active);
		}
		else if (getdmastate(num) == DmaState::Active)
		{
		    executedma(num);

		    if (getdmaunits(num) == 0)
		    {
		    	dmapc = memarm.getreg(15);

			if ((num == 2) && (dma2fifo == true) && (dma2req == true))
			{
			    setdmastate(num, DmaState::Paused);
			    dma2req = false;
			    return;
			}

			if ((num == 1) && (dma1fifo == true) && (dma1req == true))
			{
			    setdmastate(num, DmaState::Paused);
			    dma1req = false;
			    return;
			}

			int dstcontrol = ((getdmacontrol(num) >> 5) & 0x3);
			int dmaunits = getdmalength(num);

			if (dstcontrol == 3)
			{
			    resetdmadst(num, (dmaunits << (1 + TestBit(getdmacontrol(num), 10))));
			}

			if (!TestBit(getdmacontrol(num), 9))
			{
			    enddma(num);
			}
			else
			{
			    setdmastate(num, DmaState::Paused);
			}
		    }
		}
	    }

	    void initdma(int num)
	    {
		if (getdmastate(num) == DmaState::Inactive)
		{
		    if (TestBit(getdmacontrol(num), 15))
		    {
			setupdma(num);
		    }
		}
		else
		{
		    if (TestBit(getdmacontrol(num), 15))
		    {
			setupdma(num);
		    }
		    else
		    {
			enddma(num);
		    }
		}
	    }

	    void executedma(int num)
	    {
		if ((num == 2) && (dma2fifo == true) && (dma2req == true))
		{
		    dmavalue = readLong(getdmasrc(num));
		    writeLong(getdmadst(num), dmavalue);
		    dma2src += 4;
		    decdmaunits(num);
		    return;
		}

		if ((num == 1) && (dma1fifo == true) && (dma1req == true))
		{
		    dmavalue = readLong(getdmasrc(num));
		    writeLong(getdmadst(num), dmavalue);
		    dma1src += 4;
		    decdmaunits(num);
		    return;
		}

		if (TestBit(getdmacontrol(num), 10))
		{
		    int srccontrol = ((getdmacontrol(num) >> 7) & 0x3);
		    int dstcontrol = ((getdmacontrol(num) >> 5) & 0x3);

		    dmavalue = readLong(getdmasrc(num));
		    writeLong(getdmadst(num), dmavalue);
		    
		    incdma(num, dstcontrol, true);
		    incdma(num, srccontrol, false);
		}
		else
		{
		    int srccontrol = ((getdmacontrol(num) >> 7) & 0x3);
		    int dstcontrol = ((getdmacontrol(num) >> 5) & 0x3);

		    dmavalue = readWord(getdmasrc(num));
		    writeWord(getdmadst(num), dmavalue);

		    incdma(num, dstcontrol, true);
		    incdma(num, srccontrol, false);
		}		

		decdmaunits(num);
	    }

	    bool dmainprogress()
	    {
		bool temp = false;

		for (int i = 0; i < 4; i++)
		{
		    temp |= dmainprogress(i);
		}

		return temp;
	    }

	    bool dmainprogress(int num)
	    {
		return ((getdmastate(num) == DmaState::Starting) || (getdmastate(num) == DmaState::Active));
	    }

	    void updatedma()
	    {
		for (int i = 0; i < 4; i++)
		{
		    if (dmainprogress(i))
		    {
		        updatedma(i);
			return;
		    }
		}
	    }

	    bool loadBIOS(string filename);
	    bool loadROM(string filename);

	    bool loadbackup(string filename);
	    bool savebackup(string filename);

	    enum CartridgeType : int
	    {
		None = -1,
		CartSRAM = 0,
		CartEEPROM = 1,
		CartFlash128K = 2,
		CartFlash64K = 3,
	    };

	    CartridgeType carttype;

	    CartridgeType getcarttype(vector<uint8_t> &rom)
	    {
		CartridgeType type = CartridgeType::None;

		for (int i = 0; i < (int)(rom.size()); i++)
		{
		    switch (rom[i])
		    {
			case 0x45:
			{
			    if ((strncmp((const char*)&rom[i], "EEPROM", 6) == 0))
			    {
				cout << "EEPROM" << endl;
				type = CartridgeType::CartEEPROM;
				return type;
			    }
			}
			break;
			case 0x46:
			{
			    if ((strncmp((const char*)&rom[i], "FLASH_", 6) == 0))
			    {
				cout << "FLASH 64K" << endl;
				type = CartridgeType::CartFlash64K;
				return type;
			    }
			    else if ((strncmp((const char*)&rom[i], "FLASH512", 8) == 0))
			    {
				cout << "FLASH 64K" << endl;
				type = CartridgeType::CartFlash64K;
				return type;
			    }
			    else if ((strncmp((const char*)&rom[i], "FLASH1M", 7) == 0))
			    {
				cout << "FLASH 128K" << endl;
				type = CartridgeType::CartFlash128K;
				return type;
			    }
			}
			break;
			case 0x53:
			{
			    if ((strncmp((const char*)&rom[i], "SRAM", 4) == 0))
			    {
				cout << "SRAM" << endl;
				type = CartridgeType::CartSRAM;
				return type;
			    }
			}
			break;
		    }
		}

		return type;
	    }
		
		uint8_t readopenbus(uint32_t addr)
		{
			uint8_t temp = 0;
			
			uint32_t armtemp = 0;
			
			if (memarm.instmode == memarm.armmode)
			{
				armtemp = readLong((memarm.getreg(15) + 8));
			}
			else
			{
				uint16_t thumbtemp = readWord((memarm.getreg(15) + 4));
				armtemp = ((thumbtemp << 16) | thumbtemp);
			}
			
			int addrtemp = (addr & 0x3);
			temp = ((armtemp >> (addrtemp << 3)) & 0xFF);
			return temp;
		}
    };
};

#endif // LIBMBMETEOR_MMU
