#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cmath>
#include <fstream>
#ifndef UNHOLY
#include <CL/cl.hpp>
#endif

#include "GameViewController.h"

static HBViewMode finishedView = kHBNoView;

static void send_disconnect_message() {
    char q = 0;
    sc_static->add(&q, 1);
    sc_static->send();
}

class quitfunc {
	public:
	quitfunc() {}
	quitfunc(GameViewController* gvc) {
		_gvc = gvc;
	}
	GameViewController* _gvc;
	bool operator()(voidtype) {
		return _gvc->quit();
	}
};

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

void GameViewController::_initMenus() {
	mainmenu = new menu();
	mainmenu->add_menuitem(new actionmenuitem(leavefunc(this), (char *)"Leave Game"));
#ifndef __APPLE__
	mainmenu->add_menuitem(new togglemenuitem((char*)"Fullscreen", false, action_toggle_fullscreen));
#endif
	mainmenu->add_menuitem(new actionmenuitem(quitfunc(this), (char *)"Quit"));
}

void GameViewController::_initSound() {
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

GameViewController::GameViewController() {
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
	
	int u[2];
	bool done = false;
	for (int i = 0; i < 10 && !done; i++) {
		WritePacket* wp = new WritePacket(CTS_CONNECT, 0); 
        sc->send(wp);
        delete wp;

		SDL_Delay(200);
		ReadPacket* rp;
		while ((rp = sc->receive((char*)&u, 8)) != NULL) {
			if (rp->message_type == STC_INITIAL_ANGLE) {
				done = true;
				break;
			}
            delete rp;
		}
	}
	if (!done) {
		cout << "Failed to connect" << endl;
		leave();
	}

	angle = rp->read_float();
    delete rp;

    myId = -1;
}

GameViewController::~GameViewController() {
	finishedView = kHBNoView;
	SDL_ShowCursor(true);
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	delete sc;
	delete mainmenu;
}

HBViewMode GameViewController::didFinishView() {
	return finishedView;
}

void GameViewController::process() {
	int status;
	
	int count = 0, oldTime = SDL_GetTicks();
	// seems like this is crashing on the second time it's called
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

#ifdef UNHOLY
void GameViewController::_initGL() {
#ifndef __APPLE__
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		cout << "glewInit failed; " << glewGetErrorString(err) << "\n";
		exit(-1);
	}
#endif

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ((float) WIDTH) / ((float) HEIGHT), 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	quad = gluNewQuadric();
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

	GLfloat light_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat light_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular[] = { 0.8, 0.8, 0.8, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0);

	glEnable(GL_LIGHTING); // enable lighting 
	glEnable(GL_LIGHT0); // enable light 0

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void GameViewController::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_MULTISAMPLE_ARB);

	glLoadIdentity();
	
	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < world.minX+6) focusx = world.minX+6;
	if (focusx > world.maxX-6) focusx = world.maxX-6;
	if (focusy < world.minY+6) focusy = world.minY+6;
	if (focusy > world.maxY-6) focusy = world.maxY-6;
	
	gluLookAt(focusx-6*cos(angle), focusy-6*sin(angle), 3, focusx, focusy, 0.0, 0.0, 0.0, 1.0);
	float matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	

	//glUseProgram(program);
	//glUniform3f(glGetUniformLocation(program, "lightv"), 5*matrix[8]+matrix[12], 5*matrix[9]+matrix[13], 5*matrix[10]+matrix[14]);

	glPushMatrix();
		glScalef(1, 1, -1);
		_drawObjects();
		_drawWalls();
	glPopMatrix();

	glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		_drawFloor(.82);
	glDisable(GL_BLEND);

	_drawObjects();
	_drawWalls();

	glDisable(GL_MULTISAMPLE_ARB);

	glFlush();

	if (mainmenu->is_active()) {
		glDisable(GL_LIGHTING);
		mainmenu->drawMenu();
		glEnable(GL_LIGHTING);
	}
}

#else

void GameViewController::_initGL() {
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
#ifdef __APPLE__
	/* based on the Apple OpenCL documentation */
	// declare memory to hold the device id
	cl_device_id device_id;
	
	// ask OpenCL for exactly 1 GPU
	cl_int err = clGetDeviceIDs(
								NULL,				// platform ID
								CL_DEVICE_TYPE_GPU,	// look only for GPUs
								1,					// return an ID for only one GPU
								&device_id,			// on return, the device ID
								NULL);				// don't return the number of devices
	
	// make sure nothing went wrong
	if (err != CL_SUCCESS) {
		cout << "Something, somewhere went terribly wrong.\n";
		exit(1);
	}
	
	context = clCreateContext(
							  0, 
							  numDevices,	// the number of devices in the devices parameter
							  &devices,		// a pointer to the list of device IDs from clGetDeviceIDs
							  NULL,			// a pointer to an error notice callback function (if any)
							  NULL,			// data to pass as a param to the callback function
							  &err);		// on return, points to a result code
	
	// again, make sure nothing went wrong
	if (err != CL_SUCCESS) {
		cout << "Something, somewhere went terribly wrong.\n";
		exit(1);
	}
#else
	cl_context_properties props[7] = {
		CL_CONTEXT_PLATFORM, 
		(cl_context_properties)(platforms[0])(), 
		CL_GLX_DISPLAY_KHR, 
		(intptr_t)glXGetCurrentDisplay(), 
		CL_GL_CONTEXT_KHR, 
		(intptr_t)glXGetCurrentContext(), 
		0 
	};
	context = cl::Context(CL_DEVICE_TYPE_GPU, props, NULL, NULL, NULL);
	devices = context.getInfo<CL_CONTEXT_DEVICES>();
#endif
	if (devices.size() <=0) {
		cout << "No OpenCL devices found. Quitting." << endl;
		exit(1);
	}
	string clCode;
	{
		ifstream code("render.cl");
		clCode = string((std::istreambuf_iterator<char>(code)), std::istreambuf_iterator<char>());
	}
	cl::Program::Sources clSource(1, pair<const char*, int>(clCode.c_str(), clCode.length()+1));
	program = cl::Program(context, clSource);
	if (program.build(devices, "-I.")) {
		cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << endl;
		exit(1);
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	glOrtho(-1, 1, -1, 1, -1, 1);
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	igl = cl::Image2DGL(context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, texture, NULL);
	bs.push_back(igl);
	cq = cl::CommandQueue(context, devices[0], 0, NULL);
	glDisable(GL_TEXTURE_2D);
}

void GameViewController::render() {
	glDisable(GL_DEPTH_TEST);
	if (myId == -1) {
		glClear(GL_COLOR_BUFFER_BIT);
		return;
	}
	
	float focusx = world.objects[myId].p.x, focusy = world.objects[myId].p.y;
	if (focusx < world.minX+14) focusx += (14-focusx+world.minX)*(14-focusx+world.minX)/28.0;
	if (focusx > world.maxX-14) focusx -= (14+focusx-world.maxX)*(14+focusx-world.maxX)/28.0;
	if (focusy < world.minY+14) focusy += (14-focusy+world.minY)*(14-focusy+world.minY)/28.0;
	if (focusy > world.maxY-14) focusy -= (14+focusy-world.maxY)*(14+focusy-world.maxY)/28.0;
	
	float obspoints[4*world.obstacles.size()];
	unsigned char obscolor[4*world.obstacles.size()];
	int ti, i2 = 0;
	for (ti = 0; ; ti++) {
		while (i2 != world.obstacles.size()) {
			Vector2D diff = Vector2D(focusx, focusy)-world.obstacles[i2].p1;
			Vector2D obsdir = world.obstacles[i2].p2-world.obstacles[i2].p1;
			float smallest;
			if (diff*obsdir <= 0) smallest = diff*diff;
			else if (diff*obsdir >= obsdir*obsdir) smallest = (diff-obsdir)*(diff-obsdir);
			else smallest = diff*diff-(diff*obsdir)*(diff*obsdir)/(obsdir*obsdir);
			if (smallest <= 56*56*(1+float(WIDTH)*float(WIDTH)/float(HEIGHT)/float(HEIGHT)/4.0)) break;
			i2++;
		}
		if (i2 == world.obstacles.size()) break;
		obspoints[4*ti] = world.obstacles[i2].p1.x;
		obspoints[4*ti+1] = world.obstacles[i2].p1.y;
		obspoints[4*ti+2] = world.obstacles[i2].p2.x;
		obspoints[4*ti+3] = world.obstacles[i2].p2.y;
		obscolor[4*ti] = world.obstacles[i2].color.r;
		obscolor[4*ti+1] = world.obstacles[i2].color.g;
		obscolor[4*ti+2] = world.obstacles[i2].color.b;
		obscolor[4*ti+3] = world.obstacles[i2].color.a;
		i2++;
	}
	cl::Buffer obspointsbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(float), obspoints, NULL);
	cl::Buffer obscolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.obstacles.size()*sizeof(char), obscolor, NULL);
	
	float objpoint[2*world.objects.size()];
	float objsize[2*world.objects.size()];
	unsigned char objcolor[4*world.objects.size()];
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
	cl::Buffer objpointbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objpoint, NULL);
	cl::Buffer objsizebuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 2*world.objects.size()*sizeof(float), objsize, NULL);
	cl::Buffer objcolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.objects.size()*sizeof(char), objcolor, NULL);
	
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
	cl::Buffer lightposbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 3*world.lights.size()*sizeof(float), lightpos, NULL);
	cl::Buffer lightcolorbuf(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, 4*world.lights.size()*sizeof(char), lightcolor, NULL);
	
	cq.enqueueAcquireGLObjects(&bs);
	cl::Kernel renderKern(program, "render", NULL);
	renderKern.setArg(0, focusx-7*cos(angle));
	renderKern.setArg(1, focusy-7*sin(angle));
	renderKern.setArg(2, 4.0f);
	renderKern.setArg(3, cos(angle));
	renderKern.setArg(4, sin(angle));
	renderKern.setArg(5, -4.0f/7);
	renderKern.setArg(6, ti);
	renderKern.setArg(7, obspointsbuf);
	renderKern.setArg(8, obscolorbuf);
	renderKern.setArg(9, si);
	renderKern.setArg(10, objpointbuf);
	renderKern.setArg(11, objsizebuf);
	renderKern.setArg(12, objcolorbuf);
	renderKern.setArg(13, (int)world.lights.size());
	renderKern.setArg(14, lightposbuf);
	renderKern.setArg(15, lightcolorbuf);
	renderKern.setArg(16, igl);
	renderKern.setArg(17, WIDTH);
	renderKern.setArg(18, HEIGHT);
	renderKern.setArg(19, world.minX);
	renderKern.setArg(20, world.maxX);
	renderKern.setArg(21, world.minY);
	renderKern.setArg(22, world.maxY);
	cq.enqueueNDRangeKernel(renderKern, cl::NullRange, cl::NDRange((WIDTH+15)/16*16, (HEIGHT+15)/16*16), cl::NDRange(16, 16));
	cq.enqueueReleaseGLObjects(&bs);
	cq.finish();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
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
}
#endif

void GameViewController::_drawWalls() {
	// obstaclesGameViewController* gvc;
	glUseProgram(0);
	glBegin(GL_QUADS);
	for (vector<Obstacle>::iterator i = world.obstacles.begin(); i != world.obstacles.end(); i++) {
		glColor3f(i->color.r/255.0, i->color.g/255.0, i->color.b/255.0);
		glVertex3f(i->p1.x, i->p1.y, 0);
		glVertex3f(i->p2.x, i->p2.y, 0);
		glVertex3f(i->p2.x, i->p2.y, 1);
		glVertex3f(i->p1.x, i->p1.y, 1);
	}
	glEnd();
}

void GameViewController::_drawObjects() {
	glEnable(GL_NORMALIZE);
	int howMany = 0;
	for (map<int, Object>::iterator i = world.objects.begin(); i != world.objects.end(); i++) {
		glPushMatrix();
		glTranslatef(i->second.p.x, i->second.p.y, 0);
		glScalef(i->second.rad, i->second.rad, i->second.hrat*i->second.rad);
		glColor3f(i->second.color.r/255.0, i->second.color.g/255.0, i->second.color.b/255.0);
		gluSphere(quad, 1.0, 50, 50);
		glPopMatrix();
	}
	glDisable(GL_NORMALIZE);
}

void GameViewController::_drawFloor(float alpha) {
	// checkerboard
	unsigned int GridSizeX = world.maxX/3;
	unsigned int GridSizeY = world.maxY/3;
	unsigned int SizeX = 6;
	unsigned int SizeY = 6;

	glEnable(GL_NORMALIZE);
	glBegin(GL_QUADS);
	for (int x = 0; x < GridSizeX; ++x) {
		for (int y = 0; y < GridSizeY; ++y) {
			if (abs(x+y) & 1) {//modulo 2
				glColor4f(1.0f,1.0f,1.0f, alpha); //white
			} else {
				glColor4f(0.0f,0.0f,0.0f, alpha); //black
			}

			glNormal3f(                  0,                  0, 1);
			glVertex3f(    x*SizeX + world.minX,    y*SizeY + world.minY, 0);
			glVertex3f((x+1)*SizeX + world.minX,    y*SizeY + world.minY, 0);
			glVertex3f((x+1)*SizeX + world.minX,(y+1)*SizeY + world.minY, 0);
			glVertex3f(    x*SizeX + world.minX,(y+1)*SizeY + world.minY, 0);

		}
	}
	glEnd();
	glDisable(GL_NORMALIZE);
}

void GameViewController::_disconnect() {
	if (sc) {
		char q = 0;
		sc->add(&q, 1);
		sc->send();
	}
}

bool GameViewController::quit() {
	_disconnect();
	exit(0);
}

bool GameViewController::leave() {
	_disconnect();
	return finishedView = kHBServerConnectView;
}
