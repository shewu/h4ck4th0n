Holy Balls: The Game
A free and open source capture-the-flag game.
Version 1.0, now with gameplay *and* some semblance of object oriented code!

TODO
====
Proposed Gameplay
-----------------
    - Capture the flag based
    - event engine interfaces with game engine that announces what happens, sends data to socket code to talk to client

Requirements
============
* Linux
	- Hardware
		+ unholyballs requires at least a modern integrated graphics chipset (OpenGL 2.0 support)
		+ holyballs require at least an ATi Mobility Radeon HD 4670 to run at 640x480 at 60fps
	- Software
		+ SDL2 is required for both versions
	    + OpenAL is required for both versions
	    + OpenGL and GLEW is required for both versions
	    + OpenCL is required only for holyballs
* The Mac version works on a Mac running Mac OS X 10.10 Yosemite or higher. Earlier versions have not been tested.

Building
=======
* Linux
	- make
		see 'make all'

	- make server
		will build only the server

	- make holyballs
		will build holyballs, the ray tracing binary. requires OpenCL and at minimum a GTS450. will only work for single GPU systems.
	
	- make multiballs
		will build multiballs, the multi-gpu ray tracing binary. requires OpenCL and at minimum a GTS450. if on a single GPU system, will run slower than holyballs.

	- make unholyballs
		will build only the unholyballs binary, for people with lesser graphics cards

	- make all
		will build all four binaries
* Mac
    - Open `Holy Balls.xcodeproj` and build `hbserver` and one of the clients.
    - Copy the sound filles, font files, and optionally the OpenCL sources (required for holyballs and multiballs) into your build directory.

Run
===
Linux
-----
-h: to display help message

-i [ip]: to connect to specified server; default is 127.0.0.1

-d [width] [height]: to play at specified resolution; default is 640x480

-f: to play in fullscreen; will not work if specified resolution is greater than screen resolution

Playing
=======
* Mouse: Turn
* W: Accelerate forwards
* A: Accelerate left
* S: Accelerate backwards
* D: Accelerate right
* Escape: Bring up menu

Credits
=======
* Sounds from freesound.org
	- bounce sound from user dreamoron
	- ding sound from user acclivity
	- splat sound from user benhillyard

Style Guide
===========
https://google-styleguide.googlecode.com/svn/trunk/cppguide.html
