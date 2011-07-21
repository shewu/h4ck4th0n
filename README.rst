Holy Balls: The Game
A free and open source capture-the-flag game.
Version 0.4, now with gameplay and a menu *and* Mac OS X support!

Game
====

Requirements
------------
* Linux
  * Hardware
    * unholyballs requires at least a modern integrated graphics chipset
    * holyballs require at least an ATi Mobility Radeon HD 4670 to run at 640x480 at 60fps
  * Software
	* SDL is required for both versions
    * OpenAL is required for both versions
    * OpenGL and GLEW is required for both versions
    * OpenCL is required only for holyballs
* The Mac version requires an Intel Mac running Mac OS X 10.5 Leopard or higher.

Building
--------
Linux
~~~~~
	- make
		see 'make all'

	- make server
		will build only the server

	- make holyballs
		will build holyballs, the ray tracing binary. requires OpenCL and at minimum a GTS450

	- make unholyballs
		will build only the unholyballs binary, for people with lesser graphics cards

	- make all
		will build all three binaries

Mac 
~~~
	open the .xcodeproj file in Xcode and hit the build button.

Run
---
Linux
~~~~~
-h
	to display help message

-i [ip]
	to connect to specified server; default is 127.0.0.1

-d [width] [height]
	to play at specified resolution; default is 640x480

-f
	to play in fullscreen; will not work if specified resolution is greater than screen resolution

Mac
~~~
double click on the .app to run or click the Run or Build and Run button in Xcode

Playing
-------
Mouse: Turn
W: Accelerate forwards
A: Accelerate left
S: Accelerate backwards
D: Accelerate right
Escape: Bring up menu

Credits
-------
* Sounds from freesound.org
  * bounce sound from user dreamoron
  * ding sound from user acclivity
  * splat sound from user benhillyard

Map Editor
==========

Requirements
------------
* Hardware
  * Anything that runs a JVM, really
* Software
  * Something with a recent-ish JVM

Controls
--------
WASD: Move cursor
P: Write to file "output" on disk
K: Toggle Sticky Wall mode
X: Toggle Delete mode
Space: Toggle Edit mode

