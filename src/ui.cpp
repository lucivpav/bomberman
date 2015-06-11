#include "ui.h"

#include <ncurses.h>
#include <cassert>

void UI::init()
{
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(FALSE);
    timeout(33);
}


void UI::deinit()
{
    endwin();
}

UI::MenuItem::MenuItem()
{

}

UI::MenuItem::~MenuItem()
{

}

UI::Button::Button(const char *name)
    :mName(name)
{

}

UI::Button::~Button()
{

}

bool UI::Button::keyEvent(int key)
{
    if ( key == '\n' )
        return action();
    return false;
}

std::string UI::Button::drawEvent(bool selected)
{
    return mName;
}

UI::Menu::Menu(const char * name,
               std::function<bool(void)> * loopTill)
    :mPos(1),
      mExpired(false),
      mName(name),
      mLoopTill(loopTill)
{

}

UI::Menu::~Menu()
{
    for ( auto & item : mItems )
        delete item;
}

void UI::Menu::addItem(MenuItem * item)
{
    mItems.push_back(item);
}

void UI::Menu::loop()
{
    while(1)
    {
        if ( mExpired )
            break;

        if ( mLoopTill && (*mLoopTill)() )
            break;

        int c = getch();
        if ( c != ERR )
            keyEvent(c);
        erase();
        drawEvent();
        refresh();
    }
}

void UI::Menu::keyEvent(int key)
{
    assert ( mItems.size() );

    if ( key == KEY_DOWN /*|| key == 's'*/ )
    {
        if ( mPos < (int)mItems.size() )
            mPos++;
    }
    else if ( key == KEY_UP /*|| key == 'w'*/ )
    {
        if ( mPos > 1 )
            mPos--;
    }
    else if ( key == 'q' )
    {
        mExpired = true;
    }
    else
    {
        mExpired = mItems[mPos-1]->keyEvent(key);
    }
}

void UI::Menu::drawEvent()
{
    assert ( mItems.size() );

    int width, height;
    getmaxyx(stdscr, height, width);

    int y = ( height - mItems.size() ) / 2;
    int i = 1;
    for ( auto & item : mItems )
    {
        if ( i == mPos )
        {
            std::string toDraw = item->drawEvent(true);
            int x = ( width - toDraw.size() ) / 2;
            mvprintw(y, x - 3, ("-> " + toDraw).c_str());
        }
        else
        {
            std::string toDraw = item->drawEvent(false);
            int x = ( width - toDraw.size() ) / 2;
            mvprintw(y, x, toDraw.c_str());
        }
        y++;
        i++;
    }

    int ytitle = ( height - mItems.size() ) / 2 - 2;
    int xtitle = ( width - mName.size() ) / 2;
    assert ( ytitle >= 0 );
    assert ( xtitle >= 0 );
    mvprintw(ytitle, xtitle, mName.c_str());
}


UI::List::List(const char *name)
    :mPos(0),
      mName(name)
{

}

UI::List::~List()
{

}

bool UI::List::keyEvent(int key)
{
    assert ( !mItems.empty() );

    if ( key == KEY_LEFT )
    {
        if ( mPos > 0 )
            mPos--;
        else
            mPos = mItems.size()-1;
    }
    else if ( key == KEY_RIGHT )
    {
        if ( mPos < mItems.size()-1 )
            mPos++;
        else
            mPos = 0;
    }
    return false;
}

std::string UI::List::drawEvent(bool selected)
{
    if ( selected )
        return mName + ": <- " + mItems[mPos] + " ->";
    else
        return mName + ":    " + mItems[mPos] + "   ";
}

void UI::List::addItem(const char *item)
{
    mItems.push_back(std::string(item));
}

std::string UI::List::curItem() const
{
    return mItems[mPos];
}


UI::InputField::InputField(const char * name,
                           const char * content,
                           int limit)
    :mName(name),
      mContent(content),
      mLimit(limit)
{

}

UI::InputField::~InputField()
{

}

bool UI::InputField::keyEvent(int key)
{
    if ( (key >= 'a' && key <= 'z')
         || (key >= 'A' && key <= 'Z')
         || (key >= '0' && key <= '9')
         || key == '.' || key == ':' || key == '/' )
    {
        if ( mContent.size() < mLimit )
            mContent += key;
    }
    /* todo: numbers */
    else if ( key == KEY_BACKSPACE )
    {
        if ( !mContent.empty() )
            mContent.pop_back();
    }
    return false;
}

std::string UI::InputField::drawEvent(bool selected)
{
    if ( selected )
        return mName + ": " + mContent + "_";
    else
        return mName + ": " + mContent + " ";
}

std::string UI::InputField::content() const
{
    return mContent;
}

UI::Notification::Notification(const char * text,
                               const char * buttonText,
                               std::function<bool(void)> * loopTill)
    :Menu(text, loopTill)
{
    if ( buttonText )
        addItem( new OkButton(buttonText) );
    else
        addItem( new OkButton("Ok") );

    loop();
}

UI::Notification::OkButton::OkButton(const char * name)
    :Button(name)
{

}

bool UI::Notification::OkButton::action()
{
    return true;
}
