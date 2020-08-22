#include <libmbmeteor/libmbmeteor.h>
#include <SDL2/SDL.h>
#include <cassert>
using namespace gba;
using namespace std;

GBACore core;

class SDL2Frontend : public mbMeteorFrontend
{
	public:
		SDL2Frontend(GBACore *cb) : core(cb)
		{
		
		}
		
		~SDL2Frontend()
		{
		
		}
		
		bool init()
		{
			if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
			{
				cout << "SDL could not be initialized! SDL_Error: " << SDL_GetError() << endl;
				return false;
			}
			
			window = SDL_CreateWindow("mbmeteor-SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (240 * scale), (160 * scale), SDL_WINDOW_SHOWN);
			
			if (window == NULL)
			{
				cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
				return false;
			}
			
			render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			SDL_RenderSetLogicalSize(render, 240, 160);
			
			if (render == NULL)
			{
				cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
				return false;
			}
			
			texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 240, 160);
			
			return true;
		}
		
		void shutdown()
		{
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(render);
			SDL_DestroyWindow(window);
			texture = NULL;
			render = NULL;
			window = NULL;
			SDL_Quit();
		}
		
		void runapp()
		{
			while (!quit)
			{
				processevents(quit);
				runmachine();
				limitframerate();
			}
		}
		
		void pixelcallback()
		{
			assert(render && texture);
			SDL_UpdateTexture(texture, NULL, core->framebuffer(), (240 * sizeof(RGB)));
			SDL_RenderClear(render);
			SDL_RenderCopy(render, texture, NULL, NULL);
			SDL_RenderPresent(render);
		}
		
		void processinput(SDL_Event evt)
		{
			if (evt.type == SDL_KEYDOWN)
			{
				switch (evt.key.keysym.sym)
				{
					case SDLK_a: core->keypressed(Button::A); break;
					case SDLK_b: core->keypressed(Button::B); break;
					case SDLK_UP: core->keypressed(Button::Up); break;
					case SDLK_DOWN: core->keypressed(Button::Down); break;
					case SDLK_LEFT: core->keypressed(Button::Left); break;
					case SDLK_RIGHT: core->keypressed(Button::Right); break;
					case SDLK_RETURN: core->keypressed(Button::Start); break;
					case SDLK_SPACE: core->keypressed(Button::Select); break;
					case SDLK_l: core->keypressed(Button::L); break;
					case SDLK_r: core->keypressed(Button::R); break;
				}
			}
			else if (evt.type == SDL_KEYUP)
			{
				switch (evt.key.keysym.sym)
				{
					case SDLK_a: core->keyreleased(Button::A); break;
					case SDLK_b: core->keyreleased(Button::B); break;
					case SDLK_UP: core->keyreleased(Button::Up); break;
					case SDLK_DOWN: core->keyreleased(Button::Down); break;
					case SDLK_LEFT: core->keyreleased(Button::Left); break;
					case SDLK_RIGHT: core->keyreleased(Button::Right); break;
					case SDLK_RETURN: core->keyreleased(Button::Start); break;
					case SDLK_SPACE: core->keyreleased(Button::Select); break;
					case SDLK_l: core->keyreleased(Button::L); break;
					case SDLK_r: core->keyreleased(Button::R); break;
				}
			}
		}
		
		void processevents(bool& quit)
		{
			while (SDL_PollEvent(&event))
			{
				processinput(event);
				if (event.type == SDL_QUIT)
				{
					quit = true;
				}	
			}
		}
		
		void runmachine()
		{
			core->runcore();
		}
		
		void limitframerate()
		{
	    	framecurrenttime = SDL_GetTicks();

	    	if ((framecurrenttime - framestarttime) < 16)
	    	{
	    		SDL_Delay(16 - (framecurrenttime - framestarttime));
	    	}

	    	framestarttime = SDL_GetTicks();

	    	fpscount++;

	    	if (((SDL_GetTicks() - fpstime) >= 1000))
	    	{
	    		fpstime = SDL_GetTicks();
				stringstream title;
	    		title << "mbmeteor-SDL2-" << fpscount << " FPS";
				SDL_SetWindowTitle(window, title.str().c_str());
				fpscount = 0;
	    	}
		}
		
		SDL_Window *window = NULL;
		SDL_Renderer *render = NULL;
		SDL_Texture *texture = NULL;
		
		int scale = 2;
		
		bool quit = false;
		SDL_Event event;
		
		GBACore *core = NULL;
		
		Uint32 framecurrenttime = 0;
		Uint32 framestarttime = 0;
	
		int fpscount = 0;
		Uint32 fpstime = 0;
};

int main(int argc, char* argv[])
{
	SDL2Frontend *front = new SDL2Frontend(&core);
	core.setfrontend(front);
	
	if (!core.getoptions(argc, argv))
	{
		return 1;
	}
	
	if (!core.initcore())
	{
		cout << "Unable to start libmbmeteor." << endl;
		return 1;
	}
	
	core.runapp();
	core.shutdowncore();
	return 0;
}
