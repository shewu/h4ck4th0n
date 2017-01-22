#include "Menu.h"

Menu::Menu() : current_index(0), is_item_active(false), is_menu_active(false) {
    setAppearanceMiddle();
}

void Menu::addMenuItem(std::unique_ptr<MenuItem>&& a) {
    menuItems.push_back(std::move(a));
}

bool Menu::isActive() {
    return is_menu_active;
}

void Menu::setActive(bool a) {
    is_menu_active = a;
    if (a)
        menuItems[current_index]->onSelect();
    else
        menuItems[current_index]->onDeselect();
}

void Menu::setAppearance(bool a, float b, float c, float d, float e) {
    transparent = a;
    x1 = b;
    x2 = c;
    y1 = d;
    y2 = e;
}
void Menu::setAppearanceFull() {
    transparent = false;
    x1 = 0.0f;
    x2 = 1.0f;
    y1 = 0.0f;
    y2 = 1.0f;
}
void Menu::setAppearanceMiddle() {
    transparent = true;
    x1 = 0.15f;
    x2 = 0.85f;
    y1 = 0.15f;
    y2 = 0.85f;
}

void Menu::keyInput(int key) {
    if (!is_menu_active)
        return;

    if (is_item_active) {
        if (!menuItems[current_index]->keyInput(key))
            is_item_active = false;
        return;
    }

    if (key == MENU_KEY_UP) {
        menuItems[current_index]->onDeselect();
        if (current_index == 0) {
            current_index = static_cast<int>(menuItems.size()) - 1;
        } else {
            current_index--;
        }
        menuItems[current_index]->onSelect();
    } else if (key == MENU_KEY_DOWN) {
        menuItems[current_index]->onDeselect();
        if (current_index == (int)menuItems.size() - 1)
            current_index = 0;
        else
            current_index++;
        menuItems[current_index]->onSelect();
    } else if (key == MENU_KEY_ENTER) {
        is_item_active = menuItems[current_index]->activate();
    } else if (key == MENU_KEY_BACK) {
        menuItems[current_index]->onDeselect();
        is_menu_active = false;
    } else {
        menuItems[current_index]->keyInputNonActive(key);
    }
}
