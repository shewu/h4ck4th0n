#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <unistd.h>

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
#include "Constants.h"

using namespace std;

int WIDTH = 640;
int HEIGHT = 480;
bool NORAPE = true;

SDL_Surface* screen;
char* ipaddy = (char *)"127.0.0.1";

void initVideo() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "Something went wrong!\n";
		exit(-1);
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
#ifdef UNHOLY
	if (!NORAPE) {
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	}
#endif
	// detect aspect ratio
	float ratio = float(SDL_GetVideoInfo()->current_w) / SDL_GetVideoInfo()->current_h;

	float d16x9 = abs(ratio - SIXTEEN_BY_NINE);
	float d16x10 = abs(ratio - SIXTEEN_BY_TEN);
	float d4x3 = abs(ratio - FOUR_BY_THREE);

	uint16_t** arr = NULL;
	if (WIDTH == -1 || HEIGHT == -1) {
		if (d16x9 < d16x10 && d16x9 < d4x3) {
			arr = reinterpret_cast<uint16_t **>(const_cast<uint16_t(*)[2]>(sixteenbynine));
		}
		else if (d16x10 < d16x9 && d16x10 < d4x3) {
			arr = reinterpret_cast<uint16_t **>(const_cast<uint16_t(*)[2]>(sixteenbyten));
		}
		else if (d4x3 < d16x10 && d4x3 < d16x9) {
			arr = reinterpret_cast<uint16_t **>(const_cast<uint16_t(*)[2]>(fourbythree));
		}

		if (arr) {
			WIDTH = arr[0][0];
			HEIGHT = arr[0][1];
		} else {
			WIDTH = 640;
			HEIGHT = 480;
		}
	}

	WIDTH = ALIGN(WIDTH);
	HEIGHT = ALIGN(HEIGHT);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_DOUBLEBUF | SDL_OPENGL);
	//SDL_ShowCursor(false);
	//SDL_WM_GrabInput(SDL_GRAB_ON);

	glViewport(0, 0, WIDTH, HEIGHT);

	cout << "using resolution " << WIDTH << "x" << HEIGHT << "\n";
	return;
}

int main(int argc, char* argv[]) {
	srand(time(NULL));
	// process args
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "-h")) {
			printf("Usage:\n"
					"-h to show this message\n"
#ifdef UNHOLY
					"-norape to avoid antialiasing\n"
#endif
					);
			exit(0);
		} else if (!strcmp(argv[i], "-d")) {
			WIDTH = ALIGN(atoi(argv[i+1]));
			HEIGHT = ALIGN(atoi(argv[i+2]));
			cout << "Playing at " << WIDTH << "x" << HEIGHT << "\n";
		} else if (!strcmp(argv[i], "-i")) {
			ipaddy = argv[i+1];
		}
#ifdef UNHOLY
		else if (!strcmp(argv[i], "-norape")) {
			NORAPE = true;
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
					SDL_GL_SwapBuffers();
				}
				delete svc;
				break;
			}
			case kHBServerConnectView: {
				ServerConnectViewController* scvc = new ServerConnectViewController();
				while ((viewToDisplay = scvc->didFinishView()) == kHBNoView) {
					scvc->process();
					scvc->render();
					SDL_GL_SwapBuffers();
				}
				delete scvc;
				break;
			}
			case kHBGameView: {
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
					SDL_GL_SwapBuffers();
				}
				delete gvc;
				break;
			}
			default: {
				break;
			}
		}
	}

	return 0;
}
