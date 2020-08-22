// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

#include "../../include/libmbmeteor/libmbmeteor.h"
using namespace gba;
using namespace std;

namespace gba
{
	mbMeteorFrontend::mbMeteorFrontend()
	{
	
	}
	
	mbMeteorFrontend::~mbMeteorFrontend()
	{
	
	}
	
	GBACore::GBACore()
	{
		corescheduler = make_unique<Scheduler>();
		coremmu = make_unique<MMU>();
		coregpu = make_unique<GPU>(*coremmu, *corescheduler);
		coreinput = make_unique<Input>(*coremmu);
		corecpu = make_unique<CPU>(*coremmu, *coregpu, *corescheduler);
	}
	
	GBACore::~GBACore()
	{
	
	}
	
	void GBACore::setfrontend(mbMeteorFrontend *cb)
	{
		front = cb;
		
		if (front != NULL)
		{
			setpixelcallback(bind(&mbMeteorFrontend::pixelcallback, front));
		}
	}
	
	bool GBACore::loadROM(string filename)
	{
		return coremmu->loadROM(filename);
	}
	
	bool GBACore::loadBIOS(string filename)
	{
		return coremmu->loadBIOS(filename);
	}
	
	bool GBACore::initcore()
	{
		coremmu->init();
		
		if (!coremmu->loadBIOS(biosname))
		{
			return false;
		}
		
		if (!coremmu->loadROM(romname))
		{
			return false;
		}
		
		corecpu->init();
		coregpu->init();
		
		cout << "mbMeteor::Initialized" << endl;
		
		if (front != NULL)
		{
			return front->init();
		}
		else
		{
			return true;
		}
	}
	
	void GBACore::shutdowncore()
	{
		coregpu->shutdown();
		corecpu->shutdown();
		coremmu->shutdown();
		
		cout << "mbMeteor::Shutting down..." << endl;
	}
	
	void GBACore::printusage(char* argv[])
	{
		cout << "Usage: " << argv[0] << " ROM BIOS" << endl;
	}
	
	bool GBACore::getoptions(int argc, char* argv[])
	{
		if (argc < 3)
		{
			printusage(argv);
			return false;
		}
		
		romname = argv[1];
		biosname = argv[2];
		return true;
	}
	
	void GBACore::runapp()
	{
		if (front != NULL)
		{
			front->runapp();
		}
	}
	
	void GBACore::runcore()
	{
		overspentcycles = corecpu->runfor((280897 + overspentcycles));
	}
	
	RGB* GBACore::framebuffer()
	{
		return coregpu->framebuffer.data();
	}
	
	void GBACore::setpixelcallback(gpufunc cb)
	{
		coregpu->setpixelcallback(cb);
	}
	
	void GBACore::keypressed(Button button)
	{
		coreinput->keypressed(button);
	}
	
	void GBACore::keyreleased(Button button)
	{
		coreinput->keyreleased(button);
	}
};
