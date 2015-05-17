#include "menu.h"

menu::menu() {
	current_index = 0;
	is_item_active = false;
	is_menu_active = false;
	setAppearanceMiddle();
}

menu::~menu() {
	menuitems.clear();
}

void menu::add_menuitem(menuitem *a) {
	menuitems.push_back(a);
}

bool menu::is_active() {
	return is_menu_active;
}

void menu::set_active(bool a) {
	is_menu_active = a;
	if(a)
		menuitems[current_index]->onSelect();
	else
		menuitems[current_index]->onDeselect();
}

void menu::setAppearance(bool a, float b, float c, float d, float e) {
	transparent = a; x1 = b; x2 = c; y1 = d; y2 = e;
}
void menu::setAppearanceFull() {
	transparent = false; x1 = 0.0f; x2 = 1.0f; y1 = 0.0f; y2 = 1.0f;
}
void menu::setAppearanceMiddle() {
	transparent = true; x1 = 0.15f; x2 = 0.85f; y1 = 0.15f; y2 = 0.85f;
}

void menu::key_input(int key) {
	if(!is_menu_active)
		return;

	if(is_item_active) {
		if(!menuitems[current_index]->key_input(key))
			is_item_active = false;
		return;
	}

	if(key == MENU_KEY_UP) {
		menuitems[current_index]->onDeselect();
		if(current_index == 0)
			current_index = (int)menuitems.size() - 1U;
		else
			current_index--;
		menuitems[current_index]->onSelect();
	}
	else if(key == MENU_KEY_DOWN) {
		menuitems[current_index]->onDeselect();
		if(current_index == (int)menuitems.size() - 1)
			current_index = 0;
		else
			current_index++;
		menuitems[current_index]->onSelect();
	}
	else if(key == MENU_KEY_ENTER) {
		menuitem *item = menuitems[current_index];
		is_item_active = item->activate();
	}
	else if(key == MENU_KEY_BACK) {
		menuitems[current_index]->onDeselect();
		is_menu_active = false;
	}
	else {
		menuitems[current_index]->key_input_non_active(key);
	}
}
