#include <cstring>

#include "Menu.h"

using std::string;

MenuItem::~MenuItem() { }
bool MenuItem::activate() { return false; }
bool MenuItem::key_input(int key) { return false; }
void MenuItem::key_input_non_active(int key) { }
bool MenuItem::shouldMenuBeDrawn() { return false; }
void MenuItem::onSelect() { }
void MenuItem::onDeselect() { }

//SubMenuItem
SubMenuItem::~SubMenuItem() { }

SubMenuItem::SubMenuItem(Menu *m, char *name) {
	this->m = m;
	this->name = string(name);
	isActive = false;
}

bool SubMenuItem::activate() {
	isActive = true;
	m->setActive(true);
	return true;
}

bool SubMenuItem::key_input(int key) {
	if(isActive)
		m->keyInput(key);
	return (isActive = m->isActive());
}

bool SubMenuItem::shouldMenuBeDrawn() {return false;}

//ActionMenuItem
ActionMenuItem::~ActionMenuItem() { }

bool ActionMenuItem::activate() {
	return init();
}

bool ActionMenuItem::shouldMenuBeDrawn() {return true;}

//InputMenuItem
InputMenuItem::~InputMenuItem() {
	delete [] input;
}

string InputMenuItem::get_input() {
	return string(input, len);
}

bool InputMenuItem::activate() {
	displayError = false;
	return true;
}

/* I see we handle characters here -- we need to handle dots for IP addresses */
bool InputMenuItem::key_input(int key) {
	if(key == MENU_KEY_BACKSPACE) {
		if(len > 0)
			input[--len] = '\0';
	} else if(key == MENU_KEY_ENTER) {
		if(vali(input)) 
			return false;
		else
			displayError = true;
	} else if(key >= MENU_KEY_A && key <= MENU_KEY_A + 25) {
		if(len < maxlen) {
			input[len++] = 'a' + key - MENU_KEY_A;
			input[len] = '\0';
		}
	} else if(key >= MENU_KEY_0 && key <= MENU_KEY_0 + 9) {
		if(len < maxlen) {
			input[len++] = '0' + key - MENU_KEY_0;
			input[len] = '\0';
		}
	} else if (key == (int)'.') {
		if (len < maxlen) {
			input[len++] = '.';
			input[len] = '\0';
		}
	}
	return true;
}

bool InputMenuItem::shouldMenuBeDrawn() {
	return true;
}

//ToggleMenuItem
ToggleMenuItem::~ToggleMenuItem() {
}

bool ToggleMenuItem::activate() {
	state = !state;
	action(state);
	return false;
}

bool ToggleMenuItem::get_state() {
	return state;
}

//SliderMenuItem
SliderMenuItem::~SliderMenuItem() {
}

bool SliderMenuItem::get_state() {
	return curstate;
}

void SliderMenuItem::key_input_non_active(int key) {
	if(key == MENU_KEY_LEFT) {
		if(newcurstate > 0) {
			newcurstate--;
			//if(action != NULL) action(curstate); //now somewhere else
		}
	}
	else if(key == MENU_KEY_RIGHT) {
		if(newcurstate < states.size() - 1) {
			newcurstate++;
			//if(action != NULL) action(curstate);
		}
	}
}

bool SliderMenuItem::activate() {
	curstate = newcurstate;
	action(curstate);
	return false;
}

void SliderMenuItem::onDeselect() {
	newcurstate = curstate;
}
