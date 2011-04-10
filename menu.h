#include <vector>
#include <SDL/SDL.h>
using namespace std;

#define MENU_KEY_UP 		SDLK_UP
#define MENU_KEY_DOWN 		SDLK_DOWN
#define MENU_KEY_ENTER 		SDLK_RETURN
#define MENU_KEY_BACK 		SDLK_BACKSPACE
#define MENU_KEY_BACKSPACE 	SDLK_BACKSPACE
#define MENU_KEY_A		SDLK_a
#define MENU_KEY_0		SDLK_0

class menuitem {
	public:
		virtual ~menuitem();

		//Return whether or not the menuitem is still active
		virtual bool activate();
		virtual bool key_input(int key);
		virtual void draw(bool selected, float x1, float y1, float width, float height, unsigned char alpha);
		virtual void drawAsActive();
	private:
		
};

class menu {
	public:
		menu();
		virtual ~menu();
		void add_menuitem(menuitem *a);
		void key_input(int key);
		bool is_active();
		void set_active(bool);
		void draw();

		void setAppearance(bool transparent,float x1,float x2,float y1,float y2);
		void setAppearanceFull();
		void setAppearanceMiddle();
	private:
		vector<menuitem*> menuitems;
		int current_index;
		bool is_item_active;
		bool is_menu_active;

		bool transparent;
		float x1, x2, y1, y2;
};

class submenuitem : public menuitem {
	public:
		virtual ~submenuitem();
		submenuitem(menu *m, char *name);

		virtual bool activate();
		virtual bool key_input(int key);
		virtual void draw(bool,float,float,float,float,unsigned char);
		virtual void drawAsActive();

	private:
		menu *m;
		bool isActive;
		char *name;
};

class actionmenuitem : public menuitem {
	public:
		virtual ~actionmenuitem();
		actionmenuitem(bool (*init)(), bool (*ki)(int), char *name);

		virtual bool activate();
		virtual bool key_input(int key);

	private:
		bool (*init)();
		bool (*ki)(int);
		char *name;
};

class inputmenuitem : public menuitem {
	public:
		inputmenuitem(  int maxInputLen, 
				bool (*inputValidator)(char *),
				char *initInput,
				char *invalidInputError);
		virtual ~inputmenuitem();
		virtual bool activate();
		virtual bool key_input(int key);
		char *get_input();
	private:
		char *input;
		int maxlen, len;

		char *invalidInputError;
		bool displayError;
		bool (*vali)(char *);
};
