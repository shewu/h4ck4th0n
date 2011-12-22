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

struct voidtype { };

template<class R, class I> class funcobj {
	public:
	virtual R operator()(I) = 0;
};

template<class R, class I, class A> class funcobjwrapper : public funcobj<R, I> {
	public:
	funcobjwrapper(A internal) {
		_internal = internal;
	}

	R operator()(I i) {
		return _internal(i);
	}
	private:
	A _internal;
};

template<class R, class I> class wrappedfuncobj {
	public:
	wrappedfuncobj() {
		real = NULL;
		refcount = new int(1);
	}
	wrappedfuncobj(const wrappedfuncobj<R, I>& toCopy) {
		real = toCopy.real;
		refcount = toCopy.refcount;
		(*refcount)++;
	}
	template<class A> wrappedfuncobj(A internal) {
		real = new funcobjwrapper<R, I, A>(internal);
		refcount = new int(1);
	}
	wrappedfuncobj& operator=(const wrappedfuncobj& toCopy) {
		(*(toCopy.refcount))++;
		if (--(*refcount) == 0) {
			delete refcount;
			if (real) delete real;
		}
		real = toCopy.real;
		refcount = toCopy.refcount;
		return *this;
	}
	R operator()(I i) {
		return (*real)(i);
	}
	~wrappedfuncobj() {
		if (--(*refcount) == 0) {
			delete refcount;
			if (real) delete real;
		}
	}
	private:
	funcobj<R, I>* real;
	int* refcount;
};

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
		vector<menuitem*> menuitems;
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
		template <class A> actionmenuitem(A init1, char *name1) {
			init = wrappedfuncobj<bool, voidtype>(init1);
			name = name1;
		}

		virtual bool activate();
		virtual bool shouldMenuBeDrawn();

	private:
		wrappedfuncobj<bool, voidtype> init;
};

class inputmenuitem : public menuitem {
	public:
		template <class A> inputmenuitem(  int maxInputLen, 
				A inputValidator,
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
	
			vali = wrappedfuncobj<bool, char*>(inputValidator);
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
		wrappedfuncobj<bool, char*> vali;
};

class togglemenuitem : public menuitem {
	public:
		template <class A> togglemenuitem(char *name1, bool state1, A act) {
			name = name1;
			state = state1;
			action = wrappedfuncobj<voidtype, bool>(act);
		}
		virtual ~togglemenuitem();
		virtual bool activate();
		bool get_state();
		virtual void draw(bool selected, float x1, float y1, float width, float height, unsigned char alpha);
	private:
		bool state;
		wrappedfuncobj<voidtype, bool> action;
		
};

class slidermenuitem : public menuitem {
	public:
		template <class A> slidermenuitem(char *name1, char** states1, int len1, int curstate1, A act) {
			name = name1;
			states = states1;
			len = len1;
			curstate = curstate1;
			newcurstate = curstate1;
			action = wrappedfuncobj<voidtype, int>(act);
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
		wrappedfuncobj<voidtype, int> action;
};

#endif
