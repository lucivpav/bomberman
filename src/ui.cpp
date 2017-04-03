#include "ui.h"

#include <cassert>
#include <iostream>

#include <ncurses.h>
#include <SDL2/SDL.h>
#include <SDL/SDL_image.h>

UI::MenuItem::MenuItem(const char * name)
  :mName(name)
{

}

UI::MenuItem::~MenuItem()
{

}

const std::string & UI::MenuItem::getName() const
{
  return mName;
}

UI::Button::Button(const char *name, std::function<bool(void)> action)
    :MenuItem(name),
      mAction(action)
{

}

UI::Button::~Button()
{

}

bool UI::Button::keyEvent(int key)
{
    if ( key == UI::KENTER )
        return mAction();
    return false;
}

std::string UI::Button::drawEventTUI(bool selected)
{
  return getName();
}

std::string UI::Button::drawEventGUI()
{

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

        int c = getch(); // if TUI
        if ( c != ERR )
            keyEvent(convertKeyTUI(c));
        erase();
        drawEventTUI();
        refresh();
    }
}

void UI::Menu::keyEvent(int key)
{
    assert ( mItems.size() );

    if ( key == UI::KDOWN )
    {
        if ( mPos < (int)mItems.size() )
            mPos++;
    }
    else if ( key == UI::KUP )
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

void UI::Menu::drawEventTUI()
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
      std::string toDraw = item->drawEventTUI(true);
      int x = ( width - toDraw.size() ) / 2;
      mvprintw(y, x - 3, ("-> " + toDraw).c_str());
    }
    else
    {
      std::string toDraw = item->drawEventTUI(false);
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

void UI::Menu::drawEventGUI()
{

}

UI::List::List(const char *name)
    : MenuItem(name),
      mPos(0)
{

}

UI::List::~List()
{

}

bool UI::List::keyEvent(int key)
{
    assert ( !mItems.empty() );

    if ( key == UI::KLEFT )
    {
        if ( mPos > 0 )
            mPos--;
        else
            mPos = mItems.size()-1;
    }
    else if ( key == UI::KRIGHT )
    {
        if ( mPos < (int)mItems.size()-1 )
            mPos++;
        else
            mPos = 0;
    }
    return false;
}

std::string UI::List::drawEventTUI(bool selected)
{
  if ( selected )
    return getName() + ": <- " + mItems[mPos] + " ->";
  else
    return getName() + ":    " + mItems[mPos] + "   ";
}

std::string UI::List::drawEventGUI()
{

}

void UI::List::addItem(const char *item)
{
    mItems.push_back(std::string(item));
}

void UI::List::setDefaultItem(int id)
{
    assert ( id >= 0 && id < (int)mItems.size() );
    mPos = id;
}

std::string UI::List::curItem() const
{
    return mItems[mPos];
}

UI::InputField::InputField(const char * name,
                           const char * content,
                           int limit)
    :MenuItem(name),
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
        if ( (int)mContent.size() < mLimit )
            mContent += key;
    }
    else if ( key == UI::KBACKSPACE )
    {
        if ( !mContent.empty() )
            mContent.pop_back();
    }
    return false;
}

std::string UI::InputField::drawEventTUI(bool selected)
{
  if ( selected )
    return getName() + ": " + mContent + "_";
  else
    return getName() + ": " + mContent + " ";
}

std::string UI::InputField::drawEventGUI()
{

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
    addItem( new Button(buttonText ? buttonText : "Ok",
                        std::bind(&Notification::confirmAction, this)) );
    loop();
}

bool UI::Notification::confirmAction()
{
    return true;
}
/*
void UI::GUI_error(const char * message)
{
  std::cerr << message << ", " << SDL_GetError() << std::endl;
  throw;
}

void UI::GUI_init()
{
  GUI_window = nullptr;
  GUI_renderer = nullptr;
  GUI_currentTexture = nullptr;

  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) GUI_error("SDL_Init failed");
  if ( !SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") )
    GUI_error("SDL_SetHint failed");

  GUI_window = SDL_CreateWindow("Bomberman",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if ( !GUI_window )
    GUI_error("SDL_CreateWindow failed");
  GUI_renderer = SDL_CreateRenderer(GUI_window, -1, SDL_RENDERER_ACCELERATED);
  if ( !GUI_renderer ) GUI_error("SDL_CreateRenderer failed");
  SDL_SetRenderDrawColor(GUI_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  int imgFlags = IMG_INIT_PNG;
  if ( !(IMG_Init(imgFlags) & imgFlags) )
    GUI_error("SDL_image init failed");
}
*/

void UI::init(bool gui_mode)
{
  UI::gui_mode = gui_mode;
  if ( gui_mode ) {
  } else {
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    curs_set(FALSE);
    timeout(33);
  }
}

void UI::deinit()
{
  if ( gui_mode ) {

  } else {
    endwin();
  }
}

int UI::convertKeyTUI(int key)
{
  switch (key) {
  case KEY_DOWN: return UI::KDOWN;
  case KEY_UP: return UI::KUP;
  case KEY_LEFT: return UI::KLEFT;
  case KEY_RIGHT: return UI::KRIGHT;
  case KEY_BACKSPACE: return UI::KBACKSPACE;
  case '\n': return UI::KENTER;
  default: return key;
  }
}
