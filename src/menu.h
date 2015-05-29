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
    virtual ~Menu();
    void addItem(MenuItem *item);
    void loop();
    virtual void keyEvent(int key);
    virtual void drawEvent();
private:
    std::vector<MenuItem*> items;
    int pos;
    bool expired;
};

#endif
