#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <unistd.h>
#include <cmath>

#include "SplashViewController.h"
#include "ServerConnectViewController.h"
#ifdef UNHOLY
#include "UnholyGameViewController.h"
#else
#ifdef MULTI
#include "MultiGameViewController.h"
#else
#include "HolyGameViewController.h"
#endif
#endif
#include "constants.h"

using namespace std;

int WIDTH = 640;
int HEIGHT = 480;
bool NOAA = false;

SDL_Window* screen;
SDL_GLContext glContext;
char* ipaddy = (char *)"127.0.0.1";

static float getAspectRatio() {
    SDL_DisplayMode current;
    int retCode = SDL_GetCurrentDisplayMode(0, &current);
    if (retCode) {
        return kHBAspectRatioFourByThree;
    }
    
    return float(current.w) / current.h;
}

static void initVideo() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Something went wrong!\n");
		exit(-1);
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifdef UNHOLY
	if (!NOAA) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}
#endif
	// detect aspect ratio
    const float ratio = getAspectRatio();
	
	if (WIDTH == -1 || HEIGHT == -1) {
        if (abs(ratio - kHBAspectRatioFourByThree) < EPS) {
            WIDTH = four_by_three[0].first;
            HEIGHT = four_by_three[0].second;
        } else if (abs(ratio - kHBAspectRatioFiveByFour) < EPS) {
            WIDTH = five_by_four[0].first;
            HEIGHT = five_by_four[0].second;
        } else if (abs(ratio - kHBAspectRatioSixteenByNine) < EPS) {
            WIDTH = sixteen_by_nine[0].first;
            HEIGHT = sixteen_by_nine[0].second;
        } else if (abs(ratio - kHBAspectRatioSixteenByTen) < EPS) {
            WIDTH = sixteen_by_ten[0].first;
            HEIGHT = sixteen_by_ten[0].second;
        } else {
			WIDTH = 640;
			HEIGHT = 480;
		}
	}
	
	WIDTH = ALIGN(WIDTH);
	HEIGHT = ALIGN(HEIGHT);
	
    screen = SDL_CreateWindow("Holy Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
    glContext = SDL_GL_CreateContext(screen);
	
	glViewport(0, 0, WIDTH, HEIGHT);
	
	cout << "using resolution " << WIDTH << "x" << HEIGHT << "\n";
	return;
}

int main(int argc, char* argv[]) {
	srand((unsigned)time(NULL));
	// process args
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-h")) {
			printf("Usage:\n"
                   "-h to show this message\n"
                   "-d <width> <height> to set the screen resolution\n"
#ifdef UNHOLY
                   "-noaa to avoid antialiasing\n"
#endif
					);
			exit(-1);
		} else if (!strcmp(argv[i], "-d")) {
			WIDTH = ALIGN(atoi(argv[i+1]));
			HEIGHT = ALIGN(atoi(argv[i+2]));
			cout << "Playing at " << WIDTH << "x" << HEIGHT << "\n";
		} else if (!strcmp(argv[i], "-i")) {
			ipaddy = argv[i+1];
		}
#ifdef UNHOLY
		else if (!strcmp(argv[i], "-noaa")) {
			NOAA = true;
		}
#endif
	}
	initVideo();
	init_font();
	cout << "Starting client\n";
	
	HBViewMode viewToDisplay = kHBMainMenuView;
	while (true) {
		switch (viewToDisplay) {
			case kHBMainMenuView: {
				SplashViewController* svc = new SplashViewController();
				while ((viewToDisplay = svc->didFinishView()) == kHBNoView) {
					svc->process();
					svc->render();
                    SDL_GL_SwapWindow(screen);
				}
				delete svc;
				break;
			}
			case kHBServerConnectView: {
				P(("drawing server connect view\n"));
				ServerConnectViewController* scvc = new ServerConnectViewController();
				while ((viewToDisplay = scvc->didFinishView()) == kHBNoView) {
					scvc->process();
					scvc->render();
                    SDL_GL_SwapWindow(screen);
				}
				delete scvc;
				break;
			}
			case kHBGameView: {
				P(("drawing game view\n"));
#ifdef UNHOLY
				UnholyGameViewController* gvc = new UnholyGameViewController();
#else
#ifdef MULTI
				MultiGameViewController* gvc = new MultiGameViewController();
#else
				HolyGameViewController* gvc = new HolyGameViewController();
#endif
#endif
				while ((viewToDisplay = gvc->didFinishView()) == kHBNoView) {
					gvc->process();
					gvc->render();
                    SDL_GL_SwapWindow(screen);
				}
				delete gvc;
				break;
			}
			default: {
                SDL_GL_DeleteContext(glContext);
				break;
			}
		}
	}

	return 0;
}

