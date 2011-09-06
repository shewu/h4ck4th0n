#ifndef __MENU_H__
#define __MENU_H__

#include <vector>
#include <SDL/SDL.h>
using namespace std;

#define MENU_KEY_LEFT		SDLK_LEFT
#define MENU_KEY_RIGHT		SDLK_RIGHT
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
		virtual void key_input_non_active(int key);
		virtual void draw(bool selected, float x1, float y1, float width, float height, unsigned char alpha);
		virtual void drawAsActive(unsigned char alpha);
		virtual bool shouldMenuBeDrawn();
		virtual void onSelect();
		virtual void onDeselect();
	protected:
		char *name;
		
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
		void drawMenu();

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
		virtual void drawAsActive(unsigned char alpha);
		virtual bool shouldMenuBeDrawn();
		virtual void draw(bool,float,float,float,float,unsigned char);

	private:
		menu *m;
		bool isActive;
};

class actionmenuitem : public menuitem {
	public:
		virtual ~actionmenuitem();
		actionmenuitem(bool (*init)(), bool (*ki)(int), char *name);

		virtual bool activate();
		virtual bool key_input(int key);
		virtual bool shouldMenuBeDrawn();

	private:
		bool (*init)();
		bool (*ki)(int);
};

class inputmenuitem : public menuitem {
	public:
		inputmenuitem(  int maxInputLen, 
				bool (*inputValidator)(char *),
				char *initInput,
				char *invalidInputError,
				char *text,
				char *name);
		virtual ~inputmenuitem();
		virtual bool activate();
		virtual bool key_input(int key);
		virtual void drawAsActive(unsigned char alpha);
		virtual bool shouldMenuBeDrawn();
		char *get_input();
	private:
		char *input, *text;
		int maxlen, len;

		char *invalidInputError;
		bool displayError;
		bool (*vali)(char *);
};

class togglemenuitem : public menuitem {
	public:
		togglemenuitem(char *name, bool defaultstate, void (*act)(bool));
		virtual ~togglemenuitem();
		virtual bool activate();
		bool get_state();
		virtual void draw(bool selected, float x1, float y1, float width, float height, unsigned char alpha);
	private:
		bool state;
		void (*action)(bool);
		
};

class slidermenuitem : public menuitem {
	public:
		slidermenuitem(char *name, char** states, int len, int defaultstate, void (*act)(int));
		virtual ~slidermenuitem();
		virtual void key_input_non_active(int key);
		bool get_state();
		virtual bool activate();
		virtual void draw(bool,float,float,float,float,unsigned char);
		virtual void onDeselect();
	private:
		char** states;
		int curstate, len, newcurstate;
		void (*action)(int);
};

#endif
