#include "menu.h"
#include <cstring>

//menuitem
menuitem::~menuitem() { }
bool menuitem::activate() { }
bool menuitem::key_input(int key) { }

//submenuitem
submenuitem::~submenuitem() { }

submenuitem::submenuitem(menu *m, char *name) {
	this->m = m;
	this->name = name;
	isActive = false;
}

bool submenuitem::activate() {
	isActive = true;
	m->set_active(true);
	return true;
}

bool submenuitem::key_input(int key) {
	if(isActive)
		m->key_input(key);
	return (isActive = m->is_active());
}

//actionmenuitem
actionmenuitem::~actionmenuitem() { }

actionmenuitem::actionmenuitem(bool (*init1)(), bool (*ki1)(int), char *name1) {
	init = init1;
	ki = ki1;
	name = name1;
}

bool actionmenuitem::activate() {
	return (*init)();
}

bool actionmenuitem::key_input(int key) {
	return (*ki)(key);
}

//inputmenuitem
inputmenuitem::~inputmenuitem() {
	delete [] input;
}

inputmenuitem::inputmenuitem(int maxInputLen, bool (*inputValidator)(char *), char *initInput, char *iie) {
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
}

char * inputmenuitem::get_input() {
	if(input == NULL)
		return NULL;
	char *ans = new char[len+1];
	strcpy(ans, input);
	return ans;
}

bool inputmenuitem::activate() {
	displayError = false;
	return true;
}

bool inputmenuitem::key_input(int key) {
	if(key == MENU_KEY_BACKSPACE) {
		if(len > 0)
			input[--len] = '\0';
	}
	else if(key == MENU_KEY_ENTER) {
		if(vali == NULL || (*vali)(input)) 
			return false;
		else
			displayError = true;
	}
	else if(key >= MENU_KEY_A && key <= MENU_KEY_A + 25) {
		if(len < maxlen) {
			input[len++] = 'a' + key - MENU_KEY_A;
			input[len] = '\0';
		}
	}
	else if(key >= MENU_KEY_0 && key <= MENU_KEY_0 + 9) {
		if(len < maxlen) {
			input[len++] = '0' + key - MENU_KEY_0;
			input[len] = '\0';
		}
	}
	return true;
}


