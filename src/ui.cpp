#include "ui.h"

#include <cassert>
#include <iostream>

#include <ncurses.h>

bool UI::mGUIMode;
UI::GUI UI::mGUI;

UI::MenuItem::MenuItem(const char * name)
  : mName(name),
    mTexture(nullptr)
{

}

UI::MenuItem::~MenuItem()
{

}

const std::string & UI::MenuItem::getName() const
{
  return mName;
}

void UI::MenuItem::updateTexture(const char *text)
{
  mTexture = UI::textToTexture(text, {0,0,0});
}

void UI::MenuItem::render(int x, int y)
{
  SDL_Rect renderQuad = { x, y, mGUI.WINDOW_WIDTH, mGUI.WINDOW_HEIGHT };
  SDL_RenderCopyEx(mGUI.renderer, mTexture, 0,
                   &renderQuad, 0.0, 0, SDL_FLIP_NONE);
}

SDL_Texture *UI::MenuItem::getTexture()
{
  return getTexture();
}

UI::Button::Button(const char *name, std::function<bool(void)> action)
    :MenuItem(name),
      mAction(action)
{
  updateTexture(name);
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

SDL_Texture * UI::Button::drawEventGUI()
{
  return getTexture();
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

        if ( mGUIMode ) {
          SDL_Event e;
          while ( SDL_PollEvent(&e) ) { // handle events in queue
            if ( e.type == SDL_QUIT ) {
              mExpired = true;
              break;
            }
            else if ( e.type == SDL_KEYDOWN )
              keyEvent(convertKeyGUI(e.key.keysym.sym));

            /* draw */
            SDL_SetRenderDrawColor(mGUI.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(mGUI.renderer);
            drawEventGUI();
            SDL_RenderPresent(mGUI.renderer);
          }
        } else {
          int c = getch();
          if ( c != ERR )
            keyEvent(convertKeyTUI(c));
          erase();
          drawEventTUI();
          refresh();
        }
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
  //SDL_RenderCopy(mGUI.renderer, mGUI.curTexture, 0, 0);
  int h= 0.1 * mGUI.WINDOW_HEIGHT;
  int w= 0.7 * mGUI.WINDOW_WIDTH;
  int hoffset = h - h/4;

  int tmp2 = (mItems.size()-1) * hoffset;
  int tmp = std::max(0, tmp2);
  int y = mGUI.WINDOW_HEIGHT/2 - (mItems.size()*h + tmp) / 2;

  int i = 1;
  for ( auto & item : mItems )
  {
    //if ( i == mPos )
    //{
      SDL_Texture * texture = item->drawEventGUI();
      int x = 0.2 * mGUI.WINDOW_WIDTH;
      SDL_Rect rect;
      rect.x = x-w/2; rect.y = y-h/2;
      rect.w = w; rect.h = h;
      SDL_SetRenderDrawColor(mGUI.renderer, 0, 0, 255, 255);
      SDL_RenderDrawRect(mGUI.renderer, &rect);
      //mvprintw(y, x - 3, ("-> " + toDraw).c_str());
    //}
    //else
    //{
    //  std::string toDraw = item->drawEventTUI(false);
    //  int x = ( mGUI.WINDOW_WIDTH - toDraw.size() ) / 2;
    //  SDL_Rect rect;
    //  rect.x = x-w/2; rect.y = y-h/2;
    //  rect.w = w; rect.h = h;
    //  SDL_SetRenderDrawColor(mGUI.renderer, 0, 0, 255, 255);
    //  SDL_RenderDrawRect(mGUI.renderer, &rect);
    //  //mvprintw(y, x, toDraw.c_str());
    //}
    y += 2*hoffset;
    i++;
  }
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

SDL_Texture *UI::List::drawEventGUI()
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

SDL_Texture *UI::InputField::drawEventGUI()
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

void UI::GUI::error(const char * message)
{
  std::cerr << message << ", " << SDL_GetError() << std::endl;
  throw;
}

void UI::initTUI()
{
  initscr();
  noecho();
  keypad(stdscr, TRUE);
  cbreak();
  curs_set(FALSE);
  timeout(33);
}

void UI::initGUI()
{
  mGUI.window = nullptr;
  mGUI.renderer = nullptr;
  mGUI.curTexture = nullptr;
  mGUI.font = nullptr;

  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) mGUI.error("SDL_Init failed");
  if ( !SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1") )
    mGUI.error("SDL_SetHint failed");

  mGUI.window = SDL_CreateWindow("Bomberman",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                mGUI.WINDOW_WIDTH, mGUI.WINDOW_HEIGHT,
                                SDL_WINDOW_SHOWN);
  if ( !mGUI.window )
    mGUI.error("SDL_CreateWindow failed");
  mGUI.renderer = SDL_CreateRenderer(mGUI.window, -1, SDL_RENDERER_ACCELERATED);
  if ( !mGUI.renderer ) mGUI.error("SDL_CreateRenderer failed");
  SDL_SetRenderDrawColor(mGUI.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  int imgFlags = IMG_INIT_PNG;
  if ( !(IMG_Init(imgFlags) & imgFlags) )
    mGUI.error("SDL_image init failed");

  /* TTF */
  if ( TTF_Init() < 0 ) {
    std::string msg = "SDL_ttf init failed. ";
    mGUI.error((msg + TTF_GetError()).c_str());
  }

  /* load font */
  mGUI.font = TTF_OpenFont("assets/lazy.ttf", 28);
  if ( !mGUI.font ) mGUI.error("TTF_OpenFont failed");
}

void UI::deinitTUI()
{
  endwin();
}

void UI::deinitGUI()
{
  TTF_CloseFont(mGUI.font);
  SDL_DestroyTexture(mGUI.curTexture);
  SDL_DestroyRenderer(mGUI.renderer);
  SDL_DestroyWindow(mGUI.window);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

UI::UI(bool gui_mode)
{
  mGUIMode = gui_mode;
  if ( mGUIMode ) initGUI();
  else initTUI();
}

UI::~UI()
{
  if ( mGUIMode ) deinitGUI();
  else deinitTUI();
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

int UI::convertKeyGUI(int key)
{
  switch (key) {
  case SDLK_DOWN: return UI::KDOWN;
  case SDLK_UP: return UI::KUP;
  case SDLK_LEFT: return UI::KLEFT;
  case SDLK_RIGHT: return UI::KRIGHT;
  case SDLK_BACKSPACE: return UI::KBACKSPACE;
  case SDLK_RETURN: return UI::KENTER;
  default: return key;
  }
}

SDL_Texture *UI::textToTexture(const char *text, SDL_Color color)
{
  SDL_Surface * s = TTF_RenderText_Solid(mGUI.font, text, color);
  if ( !s ) mGUI.error("TTF_RenderText_Solid failed");
  SDL_Texture * t = SDL_CreateTextureFromSurface(mGUI.renderer, s);
  if ( !t ) mGUI.error("SDL_CreateTextureFromSurface failed");
  SDL_FreeSurface(s);
  return t;
}
