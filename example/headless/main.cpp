#include <libmbmeteor/gba/mmu.h>
#include <libmbmeteor/gba/cpu.h>
#include <libmbmeteor/gba/gpu.h>
#include <iostream>
#include <functional>
using namespace gba;
using namespace std;
using namespace std::placeholders;

int overspentcycles = 0;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: " << argv[0] << " ROM" << endl;
		return 1;
	}
	
	MMU mem;
	Scheduler sched;
	mem.init();
	mem.loadROM(argv[1]);
	GPU gpu(mem, sched);
	CPU cpu(mem, gpu, sched);
	cpu.init();
	gpu.init();
	cpu.runfor(200);
	gpu.shutdown();
	cpu.shutdown();
	mem.shutdown();

	return 0;
}
