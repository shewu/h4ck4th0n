#include <cstring>

#include "Menu.h"

using std::string;

MenuItem::~MenuItem() { }
bool MenuItem::activate() { return false; }
bool MenuItem::keyInput(int) { return false; }
void MenuItem::keyInputNonActive(int) { }
bool MenuItem::shouldMenuBeDrawn() { return false; }
void MenuItem::onSelect() { }
void MenuItem::onDeselect() { }

//SubMenuItem
bool SubMenuItem::activate() {
	isActive = true;
	m->setActive(true);
	return true;
}

bool SubMenuItem::keyInput(int key) {
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

string InputMenuItem::getInput() {
	return string(input, len);
}

bool InputMenuItem::activate() {
	displayError = false;
	return true;
}

/* I see we handle characters here -- we need to handle dots for IP addresses */
bool InputMenuItem::keyInput(int key) {
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
			input[len++] = 'a' + static_cast<char>(key - MENU_KEY_A);
			input[len] = '\0';
		}
	} else if(key >= MENU_KEY_0 && key <= MENU_KEY_0 + 9) {
		if(len < maxlen) {
			input[len++] = '0' + static_cast<char>(key - MENU_KEY_0);
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

bool ToggleMenuItem::getState() {
	return state;
}

//SliderMenuItem
SliderMenuItem::~SliderMenuItem() {
}

bool SliderMenuItem::getState() {
	return curstate;
}

void SliderMenuItem::keyInputNonActive(int key) {
	if (key == MENU_KEY_LEFT) {
		if (newcurstate > 0) {
			newcurstate--;
		}
	}
	else if (key == MENU_KEY_RIGHT) {
		if (newcurstate < static_cast<int>(states.size()) - 1) {
			newcurstate++;
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
