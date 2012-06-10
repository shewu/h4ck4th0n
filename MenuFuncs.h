#ifndef MENUFUNCS_H
#define MENUFUNCS_H

// file for shared menu functors

#include "HBViewController.h"

class quitfunc {
	public:
		HBViewController* _vc;

		quitfunc() {}
		quitfunc(HBViewController* vc) : _vc(vc) {}
		bool operator()(voidtype) {
			return _vc->quit();
		}
};

#endif
