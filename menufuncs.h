#ifndef __MENUFUNCS_H__
#define __MENUFUNCS_H__

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

