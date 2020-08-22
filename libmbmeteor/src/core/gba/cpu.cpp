// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

#include "../../../include/libmbmeteor/gba/cpu.h"
using namespace gba;
using namespace std;

namespace gba
{
	CPUInterface::CPUInterface(MMU& mem, Scheduler& sched, GPU& lcd) : memory(mem), scheduler(sched), display(lcd)
	{
	
	}
	
	CPUInterface::~CPUInterface()
	{
	
	}
	
	CPU::CPU(MMU& mem, GPU& lcd, Scheduler& sched) : memory(mem), gpu(lcd), scheduler(sched)
	{
		inter = new CPUInterface(memory, scheduler, gpu);
		arm = &memory.memarm;
		arm->setinterface(inter);
	}
	
	CPU::~CPU()
	{
	
	}
	
	void CPU::init()
	{
		arm->enablethumb(true);
		arm->enablehighvectors(false);
		arm->init(0x8000000, 0x5F);
		arm->armreg.regs[13] = arm->armreg.regfiq[0] = arm->armreg.regabt[0] = arm->armreg.regund[0] = 0x3007F00;
		arm->armreg.regsvc[0] = 0x3007FE0;
		arm->armreg.regirq[0] = 0x3007FA0;
		cout << "CPU::Initialized" << endl;
	}
	
	void CPU::shutdown()
	{
		cout << "CPU::Shutting down..." << endl;
	}
}
