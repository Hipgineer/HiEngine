//#pragma comment(lib, "SDL2main.lib") 
//#pragma comment(lib, "SDL2.lib")

// Why couldn't build the code til copy dll file although all stuff are liked already?
#include "./../external/SDL2-2.0.20/include/SDL.h" 
#include <iostream>


// Global Parameters
int g_screenWidth = 1280;
int g_screenHeight = 720;


SDL_Window* g_window;			// window handle
SDL_Surface* screenSurface;
unsigned int g_windowId;		// window id

void processCommandlineArgs(int argc, char* argv[])
{
	/// process command line args
	for (int i = 1; i < argc; ++i)
	{
		int w = 1280;
		int h = 720;
		//if (sscanf(argv[i], "-fullscreen=%dx%d", &w, &h) == 2)
		//{
		//	g_screenWidth = w;
		//	g_screenHeight = h;
		//	g_fullscreen = true;
		//}
		//else if (strcmp(argv[i], "-fullscreen") == 0)
		//{
		//	g_screenWidth = w;
		//	g_screenHeight = h;
		//	g_fullscreen = true;
		//}
		//if (sscanf(argv[i], "-windowed=%dx%d", &w, &h) == 2)
		//{
		//	g_screenWidth = w;
		//	g_screenHeight = h;
		//	g_fullscreen = false;
		//}
		//else if (strstr(argv[i], "-windowed"))
		//{
		//	g_screenWidth = w;
		//	g_screenHeight = h;
		//	g_fullscreen = false;
		//}
	}
}

void HiEngineDeviceGet() {};

void SDLInit(const char* title)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)	// Initialize SDL's Video subsystem and game controllers
		printf("Unable to initialize SDL");

	unsigned int flags = SDL_WINDOW_RESIZABLE;

	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

	g_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		g_screenWidth, g_screenHeight, flags);

	g_windowId = SDL_GetWindowID(g_window);
}

int main(int argc, char* argv[]) // int argc, char* argv[])
{

	//processCommandlineArgs(argc, argv);

	HiEngineDeviceGet(); 
	/*
	// Choose which device run the simulation and render the frame.

	*/
	 
	std::string str;
#if HE_DX
	str = "compute: DX11";
#else
	str = "compute: CUDA ";
#endif

#if HE_DX_RENDER
	str = "Graphics: DX11";
#else
	str += "Graphics: CUDA";
#endif

	// Window Title (Program Title)
	const char* title = str.c_str();

	// Initializing the Window 
	SDLInit(title);

	//screenSurface = SDL_GetWindowSurface(g_window);

	//SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

	//SDL_UpdateWindowSurface(g_window);

	SDL_Delay(2000);

	std::cout << "Hello, world" << std::endl;

	SDL_DestroyWindow(g_window);

	SDL_Quit();

	return 0;
}