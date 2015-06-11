#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include <functional>

namespace UI
{

void init();
void deinit();

class MenuItem
{
public:
    MenuItem();
    virtual ~MenuItem();

    virtual bool keyEvent(int key) = 0; // called if selected
    virtual std::string drawEvent(bool selected) = 0;
};

class Button : public MenuItem
{
public:
    Button(const char * name);
    virtual ~Button();

    virtual bool keyEvent(int key);
    virtual std::string drawEvent(bool selected);

    virtual bool action() = 0;
private:
    std::string mName;
};

class List : public MenuItem
{
public:
    List(const char * name);
    virtual ~List();

    virtual bool keyEvent(int key);
    virtual std::string drawEvent(bool selected);

    void addItem(const char * item);
    std::string curItem() const;
private:
    std::vector<std::string> mItems;
    int mPos;
    std::string mName;
};

class InputField : public MenuItem
{
public:
    InputField(const char * name, const char * content, int limit);
    virtual ~InputField();

    virtual bool keyEvent(int key);
    virtual std::string drawEvent(bool selected);

    std::string content() const;
private:
    std::string mName;
    std::string mContent;
    int mLimit;
};

class Menu
{
public:
    Menu(const char * name,
         std::function<bool(void)> * loopTill = 0);
    virtual ~Menu();

    virtual void keyEvent(int key);
    virtual void drawEvent();

    void addItem(MenuItem *item);
    void loop();
private:
    std::vector<MenuItem*> mItems;
    int mPos;
    bool mExpired;
    std::string mName;

    std::function<bool(void)> * mLoopTill;
};

class Notification : public Menu
{
public:
    Notification(const char * text,
                 const char * buttonText = 0,
                 std::function<bool(void)> * loopTill = 0);

    virtual ~Notification() = default;
private:
    class OkButton : public Button
    {
    public:
        OkButton(const char * name);
        virtual ~OkButton() = default;

        virtual bool action();
    };
};

} // namespace UI

#endif
