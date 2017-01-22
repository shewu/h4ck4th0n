#ifndef MENU_H
#define MENU_H

#include <vector>
#include <functional>
#include <string>
#include <memory>
#include <SDL2/SDL.h>

#define MENU_KEY_LEFT SDLK_LEFT
#define MENU_KEY_RIGHT SDLK_RIGHT
#define MENU_KEY_UP SDLK_UP
#define MENU_KEY_DOWN SDLK_DOWN
#define MENU_KEY_ENTER SDLK_RETURN
#define MENU_KEY_BACK SDLK_BACKSPACE
#define MENU_KEY_BACKSPACE SDLK_BACKSPACE
#define MENU_KEY_A SDLK_a
#define MENU_KEY_0 SDLK_0

class MenuItem {
public:
    ~MenuItem();

private:
    // Return whether or not the MenuItem is still active
    virtual bool activate();
    virtual bool keyInput(int key);
    virtual void keyInputNonActive(int key);
    virtual void draw(bool selected, float x1, float y1, float width,
                      float height, unsigned char alpha);
    virtual void drawAsActive(unsigned char alpha);
    virtual bool shouldMenuBeDrawn();
    virtual void onSelect();
    virtual void onDeselect();

protected:
    std::string name_;
    MenuItem(std::string const &name) : name_(name) {}

    friend class Menu;
};

// The menu "owns" its MenuItems and will free them on destruction
class Menu {
public:
    Menu();
    virtual ~Menu();

    void addMenuItem(std::unique_ptr<MenuItem>&& a);
    void keyInput(int key);
    bool isActive();
    void setActive(bool);
    void draw();
    void drawMenu();

    void setAppearance(bool transparent, float x1, float x2, float y1,
                       float y2);
    void setAppearanceFull();
    void setAppearanceMiddle();

private:
    std::vector<std::unique_ptr<MenuItem>> menuItems;
    int current_index;
    bool is_item_active;
    bool is_menu_active;

    bool transparent;
    float x1, x2, y1, y2;
};

class SubMenuItem : public MenuItem {
public:
    ~SubMenuItem() {}
    SubMenuItem(std::unique_ptr<Menu> &&m, std::string &&name)
        : m(std::move(m)), isActive(false), MenuItem(name) {}

private:
    bool activate();
    bool keyInput(int key);
    void drawAsActive(unsigned char alpha);
    bool shouldMenuBeDrawn();
    void draw(bool, float, float, float, float, unsigned char);

    std::unique_ptr<Menu> m;
    bool isActive;
};

class ActionMenuItem : public MenuItem {
public:
    ~ActionMenuItem();
    ActionMenuItem(const std::function<bool()> &init1, std::string &&name1)
        : init(init1), MenuItem(name1) {}

private:
    bool activate();
    bool shouldMenuBeDrawn();

    std::function<bool()> init;
};

class InputMenuItem : public MenuItem {
public:
    InputMenuItem(
        int maxInputLen,
        const std::function<bool(std::string const &)> &inputValidator,
        std::string const &initInput, std::string const &iie,
        std::string const &t, std::string const &&nam)
        : MenuItem(nam) {
        maxlen = maxInputLen;
        input = new char[maxlen + 1];
        strcpy(input, initInput.c_str());
        len = strlen(input);

        vali = inputValidator;
        invalidInputError = iie;
        displayError = false;

        text = t;
    }
    ~InputMenuItem();

    std::string getInput();

private:
    bool activate();
    bool keyInput(int key);
    void drawAsActive(unsigned char alpha);
    bool shouldMenuBeDrawn();

    char *input;

    std::string text;
    size_t maxlen, len;

    std::string invalidInputError;
    bool displayError;
    std::function<bool(std::string const &)> vali;
};

class ToggleMenuItem : public MenuItem {
public:
    ToggleMenuItem(std::string &&name1, bool state1,
                   const std::function<void(bool)> &act)
        : state(state1), action(act), MenuItem(name1) {}
    ~ToggleMenuItem();

    bool getState();

private:
    bool activate();
    void draw(bool selected, float x1, float y1, float width, float height,
              unsigned char alpha);

    bool state;
    std::function<void(bool)> action;
};

class SliderMenuItem : public MenuItem {
public:
    SliderMenuItem(std::string &&name1, std::vector<std::string> states,
                   int curstate1, const std::function<void(int)> &act)
        : states(states), curstate(curstate1), action(act), MenuItem(name1) {}
    ~SliderMenuItem();

    bool getState();

private:
    void keyInputNonActive(int key);
    bool activate();
    void draw(bool, float, float, float, float, unsigned char);
    void onDeselect();

    std::vector<std::string> states;
    int curstate, newcurstate;
    std::function<void(int)> action;
};

#endif
