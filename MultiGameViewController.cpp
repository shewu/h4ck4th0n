#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include <fstream>

#include "MultiGameViewController.h"
#include "menufuncs.h"
#include "font.h"

static HBViewMode finishedView = kHBNoView;
static SocketConnection *sc_static = NULL;

static void send_disconnect_message() {
    char q = 0;
    sc_static->add(&q, 1);
    sc_static->send();
}

class leavefunc {
	public:
	leavefunc() {}
	leavefunc(GameViewController* gvc) {
		_gvc = gvc;
	}
	GameViewController* _gvc;
	bool operator()(voidtype) {
		return _gvc->leave();
	}
};

static voidtype action_toggle_fullscreen(bool b) {
	if(b) {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL | SDL_FULLSCREEN);
	} else {
		screen = SDL_SetVideoMode(WIDTH, HEIGHT, 24, SDL_OPENGL);
	}
}

static bool validator_test(char *a) {
	return a[0] == 'a';
}

void MultiGameViewController::_initMenus() {
	mainmenu = new menu();
	mainmenu->add_menuitem(new actionmenuitem(leavefunc(this), (char *)"Leave Game"));
#ifndef __APPLE__
	mainmenu->add_menuitem(new togglemenuitem((char*)"Fullscreen", false, action_toggle_fullscreen));
#endif
	mainmenu->add_menuitem(new actionmenuitem(quitfunc(this), (char *)"Quit"));
}

void MultiGameViewController::_initSound() {
#ifndef __APPLE__
	ALCdevice* dev;
	ALCcontext* con;
	
	dev = alcOpenDevice(NULL);
	con = alcCreateContext(dev, NULL);
	alcMakeContextCurrent(con);
	
	ALfloat pos[] = { 0, 0, 0 };
	ALfloat vel[] = { 0, 0, 0 };
	ALfloat ori[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
	
	alutInit(NULL, NULL);
	albuf[0] = alutCreateBufferFromFile("sounds/boing2.wav");
	albuf[1] = alutCreateBufferFromFile("sounds/splat2.wav");
	albuf[2] = alutCreateBufferFromFile("sounds/ding.wav");
	alGenSources(ALSRCS, alsrcs);
#endif
}

MultiGameViewController::MultiGameViewController() {
	finishedView = kHBNoView;
	_initGL();
	_initSound();
	_initMenus();
	SDL_ShowCursor(false);
	SDL_WM_GrabInput(SDL_GRAB_ON);
	cout << "Starting client\n";
	
	addrinfo hints, *res;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	getaddrinfo(ipaddy, "55555", &hints, &res);
	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	Socket* sock = new Socket(sockfd);
	sc = sock->connect(*res->ai_addr, res->ai_addrlen);
    sc_static = sc;
	
	int u[2];
	bool done = false;
	for (int i = 0; i < 10 && !done; i++) {
		sc->send();
		SDL_Delay(200);
		int n;
		while ((n = sc->receive((char*)&u, 8)) != -1) {
			if (n == 4) {
				done = true;
				break;
			}
		}
	}
	sc->packetnum = 10;
	if (!done) {
		cout << "Failed to connect" << endl;
		leave();
	}

	int local_order = ntohl(u[0]);
	angle = *reinterpret_cast<float*>(&local_order);

    myId = -1;
}

MultiGameViewController::~MultiGameViewController() {
	finishedView = kHBNoView;
	SDL_ShowCursor(true);
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	delete sc;
	delete mainmenu;
}

HBViewMode MultiGameViewController::didFinishView() {
	return finishedView;
}

void MultiGameViewController::process() {
	int status;
	
	int count = 0, oldTime = SDL_GetTicks();
	while ((status = world.receiveObjects(sc, myId)) != -1) {
		int time = SDL_GetTicks();
		if (((++count)%=100) == 0) {
			int time = SDL_GetTicks();
			float fps = 100000./(time - oldTime);
			printf("\r");
			cout.width(6);
			cout << (int)fps << "fps" << flush;
			oldTime = time;
		}

		if (status == 0) continue;
#ifndef __APPLE__
		for(vector<pair<char, Vector2D> >::iterator it = world.sounds.begin(); it != world.sounds.end(); it++) {
			int src = -1;
			for (int s = 0; s < ALSRCS; s++) {
				int st;
				alGetSourcei(alsrcs[s], AL_SOURCE_STATE, &st);
				if (st != AL_PLAYING) {
					src = s;
					break;
				}
			}
			if (src >= 0 && src < 3) {
				ALfloat alsrcpos[] = { it->second.x, it->second.y, 0 };
				ALfloat alsrcvel[] = { 0, 0, 0 };

				alSourcef(alsrcs[src], AL_PITCH, 1.0f);
				alSourcef(alsrcs[src], AL_GAIN, 1.0f);
				alSourcefv(alsrcs[src], AL_POSITION, alsrcpos);
				alSourcefv(alsrcs[src], AL_VELOCITY, alsrcvel);
				alSourcei(alsrcs[src], AL_BUFFER, albuf[it->first]);
				alSourcei(alsrcs[src], AL_LOOPING, AL_FALSE);
				alSourcePlay(alsrcs[src]);
			}
		}
#endif
	}
	
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					if (mainmenu->is_active()) {
						SDL_ShowCursor(false);
						SDL_WM_GrabInput(SDL_GRAB_ON);
						mainmenu->set_active(false);
					} else {
						SDL_ShowCursor(true);
						SDL_WM_GrabInput(SDL_GRAB_OFF);
						mainmenu->set_active(true);
					}
				}
				mainmenu->key_input(event.key.keysym.sym);
				break;
			case SDL_QUIT:
				quit();
				break;
			case SDL_MOUSEMOTION:
				if (mainmenu->is_active()) break;
				angle -= event.motion.xrel/400.0;

				while (angle >= 2*M_PI) angle -= 2*M_PI;
				while (angle < 0) angle += 2*M_PI;
				break;
		}
	}
	
	Uint8* keystate = SDL_GetKeyState(NULL);
	char buf[5];
	*((int*)buf) = htonl(*reinterpret_cast<int*>(&angle));
	buf[4] = 0;
	if(!mainmenu->is_active()) {
		if (keystate[SDLK_a]) buf[4] ^= 1;
		if (keystate[SDLK_d]) buf[4] ^= 2;
		if (keystate[SDLK_w]) buf[4] ^= 4;
		if (keystate[SDLK_s]) buf[4] ^= 8;
	}
	sc->add(buf, 5);
	sc->send();
	
#ifndef __APPLE__
	ALfloat alpos[] = { world.objects[myId].p.x, world.objects[myId].p.y, 0 };
	ALfloat alvel[] = { world.objects[myId].v.x, world.objects[myId].v.y, 0 };
	ALfloat alori[] = { 0.0, cos(angle), sin(angle), 0.0, 1.0, 0.0 };
	alListenerfv(AL_POSITION, alpos);
	alListenerfv(AL_VELOCITY, alvel);
	alListenerfv(AL_ORIENTATION, alori);
#endif
}

void MultiGameViewController::_initGL() {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);

	//context = cl::Context(devices, NULL, NULL, NULL, NULL);
	//devices = context.getInfo<CL_CONTEXT_DEVICES>();

	if (devices.size() <= 0) {
		cout << "No OpenCL devices found. Quitting." << endl;
		exit(1);
	}
	cout << "Number of OpenCL devices found: " << devices.size() << endl;
	for (int i = 0; i < devices.size(); i++) {
		cout << "Device " << i << ": " << devices[i].getInfo<CL_DEVICE_NAME>() << " Vendor ID: " << devices[i].getInfo<CL_DEVICE_VENDOR_ID>() <<  endl;
		vector<cl::Device> temp;
		temp.push_back(devices[i]);
		deviceContexts.push_back(cl::Context(temp, NULL, NULL, NULL, NULL));	
	}

	string clCode;
	{
		ifstream code("multirender.cl");
		clCode = string((std::istreambuf_iterator<char>(code)), std::istreambuf_iterator<char>());
	}

	cl::Program::Sources clSource(1, pair<const char*, int>(clCode.c_str(), clCode.length() + 1));
	for (int i = 0; i < devices.size(); i++) {
		devicePrograms.push_back(cl::Program(deviceContexts[i], clSource));
		devicePrograms[i].build(deviceContexts[i].getInfo<CL_CONTEXT_DEVICES>(), "-I.");
	}
	/*
	if (program.build(devices, "-I.")) {
		cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << endl;
		exit(1);
	}
	*/

	glViewport(0, 0, WIDTH, HEIGHT);
	glOrtho(-1, 1, -1, 1, -1, 1);
	glEnable(GL_TEXTURE_2D);
	glGenTextures(devices.size(), &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	pixels = new int[WIDTH * HEIGHT];
	for (int i = 0; i < devices.size(); i++) {
		int render_offset = (((i+1)*(HEIGHT/16))/devices.size()-(i*(HEIGHT/16))/devices.size())*16;
		images.push_back(cl::Image2D(deviceContexts[i], CL_MEM_WRITE_ONLY, cl::ImageFormat(CL_RGBA, CL_UNORM_INT8), WIDTH, render_offset));
		cqs.push_back(cl::CommandQueue(deviceContexts[i], devices[i], 0, NULL));
	}

	glDisable(GL_TEXTURE_2D);
}

void MultiGameViewController::render() {
	glDisable(GL_DEPTH_TEST);
	if (myId == -1) {
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}

	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < world.minX + 14) focusx += (14 - focusx + world.minX) * (14 - focusx + world.minX) / 28.0;
	if (focusx > world.maxX - 14) focusx -= (14 + focusx - world.maxX) * (14 + focusx - world.maxX) / 28.0;
	if (focusy < world.minY + 14) focusy += (14 - focusy + world.minY) * (14 - focusy + world.minY) / 28.0;
	if (focusy > world.maxY - 14) focusy -= (14 + focusy - world.maxY) * (14 + focusy - world.maxY) / 28.0;

	float obspoints[4 * world.obstacles.size()];
	unsigned char obscolor[4 * world.obstacles.size()];
	int ti, i2 = 0;
	for (ti = 0; ; ti++) {
		while (i2 != world.obstacles.size()) {
			Vector2D diff = Vector2D(focusx, focusy) - world.obstacles[i2].p1;
			Vector2D obsdir = world.obstacles[i2].p2 - world.obstacles[i2].p1;
			float smallest;
			if (diff * obsdir <= 0) smallest = diff * diff;
			else if (diff * obsdir >= obsdir * obsdir) smallest = (diff - obsdir) * (diff - obsdir);
			else smallest = diff * diff-(diff * obsdir)*(diff * obsdir)/(obsdir * obsdir);
			if (smallest <= 56 * 56 * (1 + float(WIDTH) * float(WIDTH) / float(HEIGHT) / float(HEIGHT) / 4.0)) break;
			i2++;
		}
		if (i2 == world.obstacles.size()) break;
		obspoints[4 * ti] = world.obstacles[i2].p1.x;
		obspoints[4 * ti + 1] = world.obstacles[i2].p1.y;
		obspoints[4 * ti + 2] = world.obstacles[i2].p2.x;
		obspoints[4 * ti + 3] = world.obstacles[i2].p2.y;
		obscolor[4 * ti] = world.obstacles[i2].color.r;
		obscolor[4 * ti + 1] = world.obstacles[i2].color.g;
		obscolor[4 * ti + 2] = world.obstacles[i2].color.b;
		obscolor[4 * ti + 3] = world.obstacles[i2].color.a;
		i2++;
	}
	vector<cl::Buffer> obspointsbuf, obscolorbuf, objpointbuf, objsizebuf, objcolorbuf, lightposbuf, lightcolorbuf;
	for (int i = 0; i < devices.size(); i++) {
		obspointsbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(float), obspoints, NULL));
		obscolorbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(char), obscolor, NULL));
	}

	float objpoint[2 * world.objects.size()];
	float objsize[2 * world.objects.size()];
	unsigned char objcolor[4 * world.objects.size()];
	map<int, Object>::iterator it = world.objects.begin();
	int si;
	for (si = 0; ; si++) {
		while (it != world.objects.end() && (focusx-it->second.p.x)*(focusx-it->second.p.x)+(focusy-it->second.p.y)*(focusx-it->second.p.y) > (56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second.rad)*(56*sqrt(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)+it->second.rad)) it++;
		if (it == world.objects.end()) break;
		objpoint[2*si] = it->second.p.x;
		objpoint[2*si+1] = it->second.p.y;
		objsize[2*si] = it->second.rad;
		objsize[2*si+1] = it->second.hrat;
		objcolor[4*si] = it->second.color.r;
		objcolor[4*si+1] = it->second.color.g;
		objcolor[4*si+2] = it->second.color.b;
		objcolor[4*si+3] = it->second.color.a;
		it++;
	}
	for(int i = 0; i < devices.size(); i++) {
		objpointbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objpoint, NULL));
		objsizebuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objsize, NULL));
		objcolorbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.objects.size()*sizeof(char), objcolor, NULL));
	}

	float lightpos[3*world.lights.size()];
	unsigned char lightcolor[4*world.lights.size()];
	for (int i = 0; i < world.lights.size(); i++) {
		lightpos[3*i] = world.lights[i].position.x;
		lightpos[3*i+1] = world.lights[i].position.y;
		lightpos[3*i+2] = world.lights[i].position.z;
		lightcolor[4*i] = world.lights[i].color.r;
		lightcolor[4*i+1] = world.lights[i].color.g;
		lightcolor[4*i+2] = world.lights[i].color.b;
		lightcolor[4*i+3] = world.lights[i].color.a;
	}
	for (int i = 0; i < devices.size(); i++) {
		lightposbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 3*world.lights.size()*sizeof(float), lightpos, NULL));
		lightcolorbuf.push_back(cl::Buffer(deviceContexts[i], CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.lights.size()*sizeof(char), lightcolor, NULL));
	}

	for (int i = 0; i < devices.size(); i++) {
		cl::Kernel renderKern(devicePrograms[i], "render", NULL);
		renderKern.setArg(0, focusx-7*cos(angle));
		renderKern.setArg(1, focusy-7*sin(angle));
		renderKern.setArg(2, 4.0f);
		renderKern.setArg(3, cos(angle));
		renderKern.setArg(4, sin(angle));
		renderKern.setArg(5, -4.0f/7);
		renderKern.setArg(6, ti);
		renderKern.setArg(7, obspointsbuf[i]);
		renderKern.setArg(8, obscolorbuf[i]);
		renderKern.setArg(9, si);
		renderKern.setArg(10, objpointbuf[i]);
		renderKern.setArg(11, objsizebuf[i]);
		renderKern.setArg(12, objcolorbuf[i]);
		renderKern.setArg(13, (int)world.lights.size());
		renderKern.setArg(14, lightposbuf[i]);
		renderKern.setArg(15, lightcolorbuf[i]);
		renderKern.setArg(16, images[i]);
		renderKern.setArg(17, WIDTH);
		renderKern.setArg(18, HEIGHT);
		renderKern.setArg(19, world.minX);
		renderKern.setArg(20, world.maxX);
		renderKern.setArg(21, world.minY);
		renderKern.setArg(22, world.maxY);
		renderKern.setArg(23, int(((i*(HEIGHT/16))/devices.size())*16));
		int render_offset = (((i+1)*(HEIGHT/16))/devices.size()-(i*(HEIGHT/16))/devices.size())*16;
		cqs[i].enqueueNDRangeKernel(renderKern, cl::NullRange, cl::NDRange(WIDTH, render_offset), cl::NDRange(16, 16));
		cl::size_t<3> offset, sz;
		offset.push_back(0);
		offset.push_back(0);
		offset.push_back(0);
		sz.push_back(WIDTH);
		sz.push_back((((i+1)*(HEIGHT/16))/devices.size()-(i*(HEIGHT/16))/devices.size())*16);
		sz.push_back(1);
		cqs[i].enqueueReadImage(images[i], false, offset, sz, 0, 0, pixels+((i*(HEIGHT/16))/devices.size())*16*WIDTH);
	}
	for (int i = 0; i < devices.size(); i++) cqs[i].finish();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(-1, -1);
		glTexCoord2f(0, 1);
		glVertex2f(-1, 1);
		glTexCoord2f(1, 1);
		glVertex2f(1, 1);
		glTexCoord2f(1, 0);
		glVertex2f(1, -1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glFlush();

	if (mainmenu->is_active()) {
		glDisable(GL_LIGHTING);
		mainmenu->drawMenu();
		glEnable(GL_LIGHTING);
	}
}

void MultiGameViewController::_disconnect() {
	if (sc) {
		char q = 0;
		sc->add(&q, 1);
		sc->send();
	}
}

bool MultiGameViewController::quit() {
	_disconnect();
	exit(0);
}

bool MultiGameViewController::leave() {
	_disconnect();
	return finishedView = kHBServerConnectView;
}

