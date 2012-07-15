#ifndef MENU_H
#define MENU_H

#include <vector>
#include <functional>
#include <SDL/SDL.h>

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

// The menu "owns" its menuitems and will free them on destruction
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
		std::vector<menuitem*> menuitems;
		int current_index;
		bool is_item_active;
		bool is_menu_active;

		bool transparent;
		float x1, x2, y1, y2;
};

// The submenuitem "owns" its menu and will free it on destruction
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
		actionmenuitem(const std::function<bool()>& init1, char *name1) {
			init = init1;
			name = name1;
		}

		virtual bool activate();
		virtual bool shouldMenuBeDrawn();

	private:
		std::function<bool()> init;
};

class inputmenuitem : public menuitem {
	public:
		inputmenuitem(  int maxInputLen, 
				const std::function<bool(char*)>& inputValidator,
				char *initInput,
				char *iie,
				char *t,
				char *nam) {
				maxlen = maxInputLen;
			input = new char[maxlen+1];
			if(initInput == NULL) {
				len = 0;
				input[0] = '\0';
			} else {
				strcpy(input, initInput);
				len = strlen(input);
			}

			vali = inputValidator;
			invalidInputError = iie;
			displayError = false;

			text = t;
			name = nam;
		}
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
		std::function<bool(char*)> vali;
};

class togglemenuitem : public menuitem {
	public:
		togglemenuitem(char *name1, bool state1, const std::function<void(bool)>& act) {
			name = name1;
			state = state1;
			action = act;
		}
		virtual ~togglemenuitem();
		virtual bool activate();
		bool get_state();
		virtual void draw(bool selected, float x1, float y1, float width, float height, unsigned char alpha);
	private:
		bool state;
		std::function<void(bool)> action;

};

class slidermenuitem : public menuitem {
	public:
		slidermenuitem(char *name1, char** states1, int len1, int curstate1, const std::function<void(int)>& act) {
			name = name1;
			states = states1;
			len = len1;
			curstate = curstate1;
			newcurstate = curstate1;
			action = act;
		}
		virtual ~slidermenuitem();
		virtual void key_input_non_active(int key);
		bool get_state();
		virtual bool activate();
		virtual void draw(bool,float,float,float,float,unsigned char);
		virtual void onDeselect();
	private:
		char** states;
		int curstate, len, newcurstate;
		std::function<void(int)> action;
};

#endif
