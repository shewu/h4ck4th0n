/*
 * a generic view controller that should be subclassed for any views.
 * TODO: separate out ViewController tasks and View tasks.
 */

class HBViewController {
	public:
		/* process is for non-drawing related tasks that need to be done every frame. */
		virtual void process() = 0;
		/* render is for all drawing-related tasks done each frame. */
		virtual void render() = 0;
};

