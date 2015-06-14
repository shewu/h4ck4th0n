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
#include "Constants.h"
#include "Util.h"

using namespace std;

int WIDTH = 640;
int HEIGHT = 480;
#ifdef UNHOLY
bool NOAA = false;
#endif

SDL_Window *screen;
SDL_GLContext glContext;
// FIXME should this really be a global variable?
string ipaddy = "127.0.0.1";

static float getAspectRatio() {
    SDL_DisplayMode current;
    int retCode = SDL_GetCurrentDisplayMode(0, &current);
    if (retCode) {
        return 0;
    }

    return float(current.w) / current.h;
}

static void initVideo() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Something went wrong!\n";
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
    const float d5x4 = fabs(ratio - FIVE_BY_FOUR);
    const float d4x3 = fabs(ratio - FOUR_BY_THREE);
    const float d16x10 = fabs(ratio - SIXTEEN_BY_TEN);
    const float d16x9 = fabs(ratio - SIXTEEN_BY_NINE);
    if (WIDTH == -1 || HEIGHT == -1) {
        int arg = argMin({d5x4, d4x3, d16x10, d16x9});
        WIDTH = RESOLUTIONS[arg].front().first;
        HEIGHT = RESOLUTIONS[arg].back().second;
    }

    WIDTH = ALIGN(WIDTH);
    HEIGHT = ALIGN(HEIGHT);

    screen = SDL_CreateWindow("Holy Balls", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT,
                              SDL_WINDOW_OPENGL);  // XXX SDL_DOUBLEBUF?
    glContext = SDL_GL_CreateContext(screen);

    glViewport(0, 0, WIDTH, HEIGHT);

    cout << "using resolution " << WIDTH << "x" << HEIGHT << "\n";
    return;
}

int main(int argc, char *argv[]) {
    srand((unsigned)time(NULL));
    // process args
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-h")) {
            printf(
                "Usage:\n"
                "-h to show this message\n"
                "-d <width> <height> to set the screen resolution\n"
#ifdef UNHOLY
                "-noaa to avoid antialiasing\n"
#endif
                );
            exit(-1);
        } else if (!strcmp(argv[i], "-d")) {
            WIDTH = ALIGN(atoi(argv[i + 1]));
            HEIGHT = ALIGN(atoi(argv[i + 2]));
            cout << "Playing at " << WIDTH << "x" << HEIGHT << "\n";
        } else if (!strcmp(argv[i], "-i")) {
            ipaddy = argv[i + 1];
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
                SplashViewController *svc = new SplashViewController();
                while ((viewToDisplay = svc->didFinishView()) == kHBNoView) {
                    svc->process();
                    svc->render();
                    SDL_GL_SwapWindow(screen);
                }
                delete svc;
                break;
            }
            case kHBServerConnectView: {
                ServerConnectViewController *scvc =
                    new ServerConnectViewController();
                while ((viewToDisplay = scvc->didFinishView()) == kHBNoView) {
                    scvc->process();
                    scvc->render();
                    SDL_GL_SwapWindow(screen);
                }
                delete scvc;
                break;
            }
            case kHBGameView: {
#ifdef UNHOLY
                UnholyGameViewController *gvc = new UnholyGameViewController();
#else
#ifdef MULTI
                MultiGameViewController *gvc = new MultiGameViewController();
#else
                HolyGameViewController *gvc = new HolyGameViewController();
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
