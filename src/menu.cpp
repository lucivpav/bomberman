#include "menu.h"

#include <ncurses.h>
#include <assert.h>

MenuItem::~MenuItem()
{
}

Menu::Menu()
    :pos(1),
      expired(false)
{
}

Menu::~Menu()
{
    for ( auto & item : items )
        delete item;
}

void Menu::addItem(MenuItem *item)
{
    items.push_back(item);
}

void Menu::loop()
{
    while(1)
    {
        if ( expired )
            break;
        int c = getch();
        if ( c != ERR )
            keyEvent(c);
        erase();
        drawEvent();
        refresh();
    }
}

void Menu::keyEvent(int key)
{
    assert ( items.size() );

    if ( key == KEY_DOWN || key == 's' )
    {
        if ( pos < items.size() )
            pos++;
    }
    else if ( key == KEY_UP || key == 'w' )
    {
        if ( pos > 1 )
            pos--;
    }
    else if ( key == '\n' )
    {
        expired = items[pos-1]->action();
    }
}

void Menu::drawEvent()
{
    assert ( items.size() );

    int width, height;
    getmaxyx(stdscr, height, width);

    int y = ( height - items.size() ) / 2;
    int i = 1;
    for ( auto & item : items )
    {
        int x = ( width - item->name().size() ) / 2;
        if ( i == pos )
            mvprintw(y, x - 3, ("-> " + item->name()).c_str());
        else
            mvprintw(y, x, item->name().c_str());
        y++;
        i++;
    }
}


