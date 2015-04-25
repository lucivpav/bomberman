#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

class MenuItem
{
public:
    MenuItem(const char * name):_name(name){}
    virtual ~MenuItem();
    virtual bool action() = 0;
    const std::string & name() const { return _name; }
private:
    std::string _name;
};

class Menu
{
public:
    Menu();
    ~Menu();
    void addItem(MenuItem *item);
    void loop();
    void keyEvent(int key);
    void drawEvent();
private:
    std::vector<MenuItem*> items;
    int pos;
    bool expired;
};

#endif