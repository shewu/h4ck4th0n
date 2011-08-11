/*
 * a generic view controller that should be subclassed for any views.
 * TODO: separate out ViewController tasks and View tasks.
 */

#ifndef HB_VIEW_CONTROLLER
#define HB_VIEW_CONTROLLER

#include "constants.h"

class HBViewController {
	public:
		virtual HBViewMode didFinishView() = 0;
		/* process is for non-drawing related tasks that need to be done every frame. */
		virtual void process() = 0;
		/* render is for all drawing-related tasks done each frame. */
		virtual void render() = 0;
};

#endif

