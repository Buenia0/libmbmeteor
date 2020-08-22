// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

#pragma once
#include <cstdint>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>
#include "libmbmeteor_api.h"
#include "gba/mmu.h"
#include "gba/cpu.h"
#include "gba/gpu.h"
#include "gba/input.h"
using namespace std;
using namespace gba;

namespace gba
{
	class LIBMBMETEOR_API mbMeteorFrontend
	{
		public:
			mbMeteorFrontend();
			~mbMeteorFrontend();
			
			virtual bool init() = 0;
			virtual void shutdown() = 0;
			virtual void runapp() = 0;
			virtual void pixelcallback() = 0;
	};

	class LIBMBMETEOR_API GBACore
	{
		public:
			GBACore();
			~GBACore();
			
	    	unique_ptr<MMU> coremmu;
	    	unique_ptr<GPU> coregpu;
	    	unique_ptr<CPU> corecpu;
	    	unique_ptr<Input> coreinput;
	    	unique_ptr<Scheduler> corescheduler;
			
			mbMeteorFrontend *front = NULL;
			
			void setfrontend(mbMeteorFrontend *cb);
			
			void printusage(char* argv[]);
			bool getoptions(int argc, char* argv[]);
			
			bool initcore();
			void shutdowncore();
			
			bool loadROM(string filename);
			bool loadBIOS(string filename);
			void setpixelcallback(gpufunc cb);
			
			RGB* framebuffer();
			
			void runapp();
			void runcore();
			
			void keypressed(Button button);
			void keyreleased(Button button);
			
			int overspentcycles = 0;
			
			string romname;
			string biosname;
	};
};
