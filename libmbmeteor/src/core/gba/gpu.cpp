// libmbmeteor
// Copyright (C) 2020 BueniaDev
//
// Licensed under the GNU GPL v3 or any later version.
// Refer to the included LICENSE file.

#include "../../../include/libmbmeteor/gba/gpu.h"
using namespace gba;
using namespace std;
using namespace std::placeholders;

namespace gba
{
	GPU::GPU(MMU& mem, Scheduler& sched) : memory(mem), scheduler(sched)
	{
		for (int i = 0; i < 0x56; i++)
		{
			mem.addiofuncs((0x4000000 + i), bind(&GPU::readgpu, this, _1), bind(&GPU::writegpu, this, _1, _2));
		}
	}
	
	GPU::~GPU()
	{
	
	}
	
	void GPU::init()
	{
		clearscreen();
		evt.delay = 0;
		setnextevent(Phase::Scanline);
		scheduler.addevent(&evt);
		cout << "GPU::Initialized" << endl;
	}
	
	void GPU::shutdown()
	{
		cout << "GPU::Shutting down..." << endl;
	}
	
	uint8_t GPU::readgpu(uint32_t addr)
	{
		uint8_t temp = 0;
		
		switch ((addr & 0xFF))
		{
			case 0x00: temp = (dispcnt & 0xFF); break;
			case 0x01: temp = (dispcnt >> 8); break;
			case 0x02: temp = 0; break;
			case 0x03: temp = 0; break;
			case 0x04: temp = (dispstat & 0xFF); break;
			case 0x05: temp = (dispstat >> 8); break;
			case 0x06: temp = (vcount & 0xFF); break;
			case 0x07: temp = 0; break;
			case 0x0E: temp = (bgcnt[3] & 0xFF); break;
			case 0x0F: temp = (bgcnt[3] >> 8); break;
			default: cout << "Unrecognized GPU read from address of " << hex << (int)((addr & 0xFF)) << endl; exit(1); break;
		}
		
		return temp;
	}
	
	void GPU::writegpu(uint32_t addr, uint8_t val)
	{
		switch ((addr & 0xFF))
		{
			case 0x00: dispcnt = ((dispcnt & 0xFF00) | val); break;
			case 0x01: dispcnt = ((dispcnt & 0xFF) | (val << 8)); break;
			case 0x02: return; break;
			case 0x03: return; break;
			case 0x04: dispstat = ((dispstat & 0xFF07) | (val & 0xF8)); break;
			case 0x05: dispstat = ((dispstat & 0xFF) | (val << 8)); break;
			case 0x06: return; break; // VCount should not be written to
			case 0x07: return; break; // VCount should not be written to
			case 0x08: bgcnt[0] = ((bgcnt[0] & 0xFF00) | val); break;
			case 0x09: bgcnt[0] = ((bgcnt[0] & 0xFF) | (val << 8)); break;
			case 0x0A: bgcnt[1] = ((bgcnt[1] & 0xFF00) | val); break;
			case 0x0B: bgcnt[1] = ((bgcnt[1] & 0xFF) | (val << 8)); break;
			case 0x0C: bgcnt[2] = ((bgcnt[2] & 0xFF00) | val); break;
			case 0x0D: bgcnt[2] = ((bgcnt[2] & 0xFF) | (val << 8)); break;
			case 0x0E: bgcnt[3] = ((bgcnt[3] & 0xFF00) | val); break;
			case 0x0F: bgcnt[3] = ((bgcnt[3] & 0xFF) | (val << 8)); break;
			case 0x10: bghoffs[0] = ((bghoffs[0] & 0x100) | val); break;
			case 0x11: bghoffs[0] = ((bghoffs[0] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x12: bgvoffs[0] = ((bgvoffs[0] & 0x100) | val); break;
			case 0x13: bgvoffs[0] = ((bgvoffs[0] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x14: bghoffs[1] = ((bghoffs[1] & 0x100) | val); break;
			case 0x15: bghoffs[1] = ((bghoffs[1] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x16: bgvoffs[1] = ((bgvoffs[1] & 0x100) | val); break;
			case 0x17: bgvoffs[1] = ((bgvoffs[1] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x18: bghoffs[2] = ((bghoffs[2] & 0x100) | val); break;
			case 0x19: bghoffs[2] = ((bghoffs[2] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x1A: bgvoffs[2] = ((bgvoffs[2] & 0x100) | val); break;
			case 0x1B: bgvoffs[2] = ((bgvoffs[2] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x1C: bghoffs[3] = ((bghoffs[3] & 0x100) | val); break;
			case 0x1D: bghoffs[3] = ((bghoffs[3] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x1E: bgvoffs[3] = ((bgvoffs[3] & 0x100) | val); break;
			case 0x1F: bgvoffs[3] = ((bgvoffs[3] & 0xFF) | ((val & 0x1) << 8)); break;
			case 0x20: return; break;
			case 0x21: return; break;
			case 0x22: return; break;
			case 0x23: return; break;
			case 0x24: return; break;
			case 0x25: return; break;
			case 0x26: return; break;
			case 0x27: return; break;
			case 0x28: return; break;
			case 0x29: return; break;
			case 0x2A: return; break;
			case 0x2B: return; break;
			case 0x2C: return; break;
			case 0x2D: return; break;
			case 0x2E: return; break;
			case 0x2F: return; break;
			case 0x30: return; break;
			case 0x31: return; break;
			case 0x32: return; break;
			case 0x33: return; break;
			case 0x34: return; break;
			case 0x35: return; break;
			case 0x36: return; break;
			case 0x37: return; break;
			case 0x38: return; break;
			case 0x39: return; break;
			case 0x3A: return; break;
			case 0x3B: return; break;
			case 0x3C: return; break;
			case 0x3D: return; break;
			case 0x3E: return; break;
			case 0x3F: return; break;
			case 0x40: return; break;
			case 0x41: return; break;
			case 0x42: return; break;
			case 0x43: return; break;
			case 0x44: return; break;
			case 0x45: return; break;
			case 0x46: return; break;
			case 0x47: return; break;
			case 0x48: return; break;
			case 0x49: return; break;
			case 0x4A: return; break;
			case 0x4B: return; break;
			case 0x4C: return; break;
			case 0x4D: return; break;
			case 0x4E: return; break;
			case 0x4F: return; break;
			case 0x50: bldcnt = ((bldcnt & 0x3F00) | val); break;
			case 0x51: bldcnt = ((bldcnt & 0xFF) << ((val & 0x3F) << 8)); break;
			case 0x52: bldalphaeva = (val & 0x1F); break;
			case 0x53: bldalphaevb = (val & 0x1F); break;
			case 0x54: bldy = (val & 0x1F); break;
			case 0x55: return; break;
			default: cout << "Unrecognized GPU write to address of " << hex << (int)((addr & 0xFF)) << endl; exit(1); break;
		}
	}
	
	void GPU::setnextevent(Phase gpuphase)
	{
		phase = gpuphase;
		evt.delay = waitcycles[phase];
	}
	
	void GPU::tick()
	{
		switch (phase)
		{
			case Phase::Scanline:
			{
				setnextevent(Phase::HBlankSearch);
				
				if (TestBit(dispstat, 4))
				{
					memory.requestinterrupt(1);
				}
				
				renderscanline();
			}
			break;
			case Phase::HBlankSearch:
			{
				setnextevent(Phase::HBlank);
				
				hblank(true);
				// cout << "Starting HBlank..." << endl;
			}
			break;
			case Phase::HBlank:
			{
				// cout << "HBlank complete..." << endl;
				hblank(false);
				vcount++;
				
				if (vcount == 160)
				{
					if (drawpixels)
					{
						drawpixels();
					}
					
					setnextevent(Phase::VBlankScanline);
					vblank(true);
					
					if (TestBit(dispstat, 3))
					{
						memory.requestinterrupt(0);
					}
				}
				else
				{
					setnextevent(Phase::Scanline);
				}
			}
			break;
			case Phase::VBlankScanline:
			{
				// cout << "Activating HBlank during VBlank..." << endl;
				setnextevent(Phase::VBlankHBlank);
				hblank(true);
				
				if (TestBit(dispstat, 4))
				{
					memory.requestinterrupt(1);
				}
			}
			break;
			case Phase::VBlankHBlank:
			{
				hblank(false);
			
				if (vcount == 227)
				{
					// cout << "Frame completed..." << endl;
					vcount = 0;
					setnextevent(Phase::Scanline);
				}
				else
				{
					// cout << "Moving to next VBlank line..." << endl;
					setnextevent(Phase::VBlankScanline);
					
					if (vcount == 226)
					{
						vblank(false);
					}
					
					vcount++;
					// cout << dec << (int)(vcount) << endl;
				}
			}
			break;
		}
	}
	
	void GPU::renderscanline()
	{
		if (((dispcnt >> 8) & 0x1F) == 0)
		{
			return;
		}
	
		switch ((dispcnt & 0x7))
		{
			case 0: renderbgmode0(); break;
			case 1: renderbgmode1(); break;
			case 3: renderbgmode3(); break;
			case 4: renderbgmode4(); break;
			default: cout << "Unrecognized GPU mode of " << dec << (int)((dispcnt & 0x7)) << endl; exit(1); break;
		}
	}
	
	void GPU::renderbgtext(int num)
	{
		if (!TestBit(dispcnt, (8 + num)))
		{
			renderbgtrans(num);
			return;
		}
	
		int sbb = ((bgcnt[num] >> 8) & 0x1F);
		int sbbblock = (sbb * 0x800);
		
		int cbb = ((bgcnt[num] >> 2) & 0x3);
		int cbbblock = (cbb * 0x4000);
		
		int tile4bit = TestBit(bgcnt[num], 7) ? 0 : 1;
		int tilesizeshift = (6 - tile4bit);
		
		int totalwidth = ((256 << TestBit(bgcnt[num], 14)) - 1);
		int totalheight = ((256 << TestBit(bgcnt[num], 15)) - 1);
		
		int ypos = ((vcount + bgvoffs[num]) & totalheight);
		
		if ((ypos & ~0xFF))
		{
			ypos &= 0xFF;
			sbbblock += (1024 << (1 + (totalwidth == 511)));
		}
		
		uint16_t tilerow = (((uint8_t)ypos >> 3) << 5);
		
		for (int pixel = 0; pixel < 240; pixel++)
		{
			int xpos = ((pixel + bghoffs[num]) & totalwidth);
			
			int map = sbbblock;
			
			if ((xpos & ~0xFF))
			{
				xpos &= 0xFF;
				map += (1024 << 1);
			}
			
			uint16_t tilecol = (xpos >> 3);
			
			uint16_t mapaddr = (map + ((tilerow + tilecol) << 1));
			uint16_t mapdata = readvram16(mapaddr);
			uint16_t tilenum = (mapdata & 0x3FF);
			
			int tileypos = (ypos & 7);
			int tilexpos = (xpos & 7);
			
			int tline = TestBit(mapdata, 11) ? (~tileypos & 7) : tileypos;
			int tcol = TestBit(mapdata, 10) ? (~tilexpos & 7) : tilexpos;
			
			uint16_t tilepos = (((tline << 3) + tcol) >> tile4bit);
			uint16_t tileaddr = (cbbblock + (tilenum << tilesizeshift) + tilepos);
			uint8_t tiledata = memory.vram[tileaddr];
			
			uint16_t palettenumber = 0;
			uint16_t paletteaddr = 0;
			
			if (TestBit(bgcnt[num], 7))
			{
				if (tiledata == 0)
				{
					bginfobuffer[num][pixel] = 0;
					continue;
				}
			
				paletteaddr = tiledata;
				palettenumber = 0;
			}
			else
			{
				int paletteindex = TestBit(xpos, 0) ? (tiledata >> 4) : (tiledata & 0xF);
				
				if (paletteindex == 0)
				{
					bginfobuffer[num][pixel] = 0;
					continue;
				}
				
				paletteaddr = paletteindex;
				palettenumber = (mapdata >> 12);
			}
			
			uint16_t color = (readpram16(palettenumber, paletteaddr) & 0x7FFF);
			bgbuffer[num][pixel] = color;
			int prior = (0x4 + (bgcnt[num] & 0x3));
			bginfobuffer[num][pixel] = prior;
		}
	}
	
	void GPU::renderbgbitmap(int num, int modenum)
	{
		if (!TestBit(dispcnt, (8 + num)))
		{
			renderbgtrans(num);
			return;
		}
	
		uint8_t line = (vcount & 0xFF);
	
		switch (modenum)
		{
			case 0:
			{		
				for (int i = 0; i < 240; i++)
				{
					int index = ((i + (line * 240)) << 1);
					uint16_t tempcolor = (memory.vram[(index + 1)] << 8) | (memory.vram[index]);
			
					uint16_t color = (tempcolor & 0x7FFF);
			
					bgbuffer[num][i] = color;
					int prior = (0x4 + (bgcnt[num] & 0x3));
					bginfobuffer[num][i] = prior;
				}
			}
			break;
			case 1:
			{
				for (int i = 0; i < 240; i++)
				{
					int pageflip = TestBit(dispcnt, 4) ? 0xA000 : 0;
					int index = ((i + (line * 240)) + pageflip);
					
					uint8_t palettecolor = memory.vram[index];
					
					if (palettecolor == 0)
					{
						bginfobuffer[num][i] = 0;
						continue;
					}
			
					uint16_t tempcolor = readpram16(0, palettecolor);
			
					uint16_t color = (tempcolor & 0x7FFF);
			
					bgbuffer[num][i] = color;
					int prior = (0x4 + (bgcnt[num] & 0x3));
					bginfobuffer[num][i] = prior;
				}
			}
			break;
			case 2: cout << "Bitmap mode 5" << endl; exit(1); break;
		}
	}
	
	void GPU::renderbgtrans(int num)
	{
		for (int i = 0; i < 240; i++)
		{
			bginfobuffer[num][i] = 0;
		}
	}
	
	void GPU::composescanline(int start, int end)
	{
		vector<int> bglayers;
		
		for (int i = end; i >= start; i--)
		{
			bglayers.push_back(i);
		}
		
		uint8_t line = (vcount & 0xFF);
		auto linebuffer = &framebuffer[(line * 240)];
		
		uint16_t backcolor = (readpram16(0, 0) & 0x7FFF);
		
		for (int i = 0; i < 240; i++)
		{
			int firstprior = 3;
			int firstlayer = 4;
			uint16_t firstcolor = backcolor;
			
			int secondprior = 3;
			int secondlayer = 4;
			uint16_t secondcolor = backcolor;
			
			for (int layer : bglayers)
			{
				int pixinfo = bginfobuffer[layer][i];
				uint16_t layercolor = bgbuffer[layer][i];
				
				if ((pixinfo & 0x4) == 0)
				{
					continue;
				}
				
				int layerprior = (pixinfo & 0x3);
				
				if (layerprior <= firstprior)
				{
					secondprior = firstprior;
					secondlayer = firstlayer;
					secondcolor = firstcolor;
					
					firstprior = layerprior;
					firstlayer = layer;
					firstcolor = layercolor;
				}
				else if (layerprior <= secondprior)
				{
					secondprior = layerprior;
					secondlayer = layer;
					secondcolor = layercolor;
				}
			}
			
			int blendspec = ((bldcnt >> 6) & 0x3);
			
			switch (blendspec)
			{
				case 0: break; // No special effect
				case 1: // Alpha blending
				{
					if (TestBit(bldcnt, firstlayer) && TestBit(bldcnt, (8 + secondlayer)))
					{
						firstcolor = alphablend(firstcolor, secondcolor);
					}
				}
				break;
				case 2: // Brightness increase
				{
					if (TestBit(bldcnt, firstlayer))
					{
						firstcolor = brightnessblend(true, firstcolor);
					}
				}
				break;
				case 3: // Brightness decrease
				{
					if (TestBit(bldcnt, firstlayer))
					{
						firstcolor = brightnessblend(false, firstcolor);
					}
				}
				break;
			}
			
			uint16_t color = firstcolor;
			
			uint8_t red = ((color & 0x1F) << 3);
			uint8_t green = (((color >> 5) & 0x1F) << 3);
			uint8_t blue = (((color >> 10) & 0x1F) << 3);
			
			RGB linecolor = {red, green, blue};
			linebuffer[i] = linecolor;
			
			if (firstlayer <= 3)
			{
				bginfobuffer[firstlayer][i] = 0;
			}
		}
	}
	
	uint16_t GPU::alphablend(uint16_t firstcol, uint16_t secondcol)
	{
		int eva = min(16, bldalphaeva);
		int evb = min(16, bldalphaevb);
		
		uint8_t red1 = (firstcol & 0x1F);
		uint8_t green1 = ((firstcol >> 5) & 0x1F);
		uint8_t blue1 = ((firstcol >> 10) & 0x1F);
		
		uint8_t red2 = (secondcol & 0x1F);
		uint8_t green2 = ((secondcol >> 5) & 0x1F);
		uint8_t blue2 = ((secondcol >> 10) & 0x1F);
		
		uint8_t red = min(31, (((red1 * eva) >> 4) + ((red2 * evb) >> 4)));
		uint8_t green = min(31, (((green1 * eva) >> 4) + ((green2 * evb) >> 4)));
		uint8_t blue = min(31, (((blue1 * eva) >> 4) + ((blue2 * evb) >> 4)));
		
		uint16_t temp = (((blue & 0x1F) << 10) | ((green & 0x1F) << 5) | (red & 0x1F));
		
		return temp;
	}
	
	uint16_t GPU::brightnessblend(bool increase, uint16_t color)
	{
		uint16_t temp = 0;
		
		int evy = min(16, bldy);
		
		uint8_t red = (color & 0x1F);
		uint8_t green = ((color >> 5) & 0x1F);
		uint8_t blue = ((color >> 10) & 0x1F);
		
		if (increase)
		{
			red += (((31 - red) * evy) >> 4);
			green += (((31 - green) * evy) >> 4);
			blue += (((31 - blue) * evy) >> 4);
		}
		else
		{
			red -= ((red * evy) >> 4);
			green -= ((green * evy) >> 4);
			blue -= ((blue * evy) >> 4);
		}
		
		temp = (((blue & 0x1F) << 10) | ((green & 0x1F) << 5) | (red & 0x1F));
		return temp;
	}
	
	void GPU::renderbgmode0()
	{
		renderbgtext(0);
		renderbgtext(1);
		renderbgtext(2);
		renderbgtext(3);
		composescanline(0, 3);
	}
	
	void GPU::renderbgmode1()
	{
		renderbgtext(0);
		renderbgtext(1);
		renderbgtrans(2);
		composescanline(0, 2);
	}
	
	void GPU::renderbgmode3()
	{
		renderbgtrans(0);
		renderbgtrans(1);
		renderbgbitmap(2, 0);
		renderbgtrans(3);
		composescanline(2, 2);
	}
	
	void GPU::renderbgmode4()
	{
		renderbgtrans(0);
		renderbgtrans(1);
		renderbgbitmap(2, 1);
		renderbgtrans(3);
		composescanline(2, 2);
	}
}
