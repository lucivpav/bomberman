#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL2/SDL_ttf.h>

/**
 *  \addtogroup UI
 *  @{
 */

/**
 * @brief The UI namespace is a collection user interfaces that can be
 * drawn onto screen and interacted with.
 * @warning The caller is responsible for calling init() prior to using
 * any of the UI components as well as calling deinit() once the caller
 * is done with it.
 */
class UI
{
private:
  void initTUI();
  void initGUI();

  void deinitTUI();
  void deinitGUI();

  struct GUI {
    SDL_Window* window;
    SDL_Renderer * renderer;
    SDL_Texture * curTexture;
    TTF_Font * font;
    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;
    void error(const char * message);
  };

public:
  static bool mGUIMode;
  static GUI mGUI;

enum KEY { KUP = -1, KDOWN = -2, KLEFT = -3, KRIGHT = -4,
           KENTER = -5, KBACKSPACE = -6 };

/**
 * @brief Initializes the UI and makes UI classes ready to use.
 */
  UI(bool gui_mode);

/**
 * @brief Deinitializes the UI and clears all it's resources.
 */
  ~UI();

static int convertKeyTUI(int key);
static int convertKeyGUI(int key);
static SDL_Texture * textToTexture(const char * text, SDL_Color color);

/**
 * @brief The MenuItem class represents a general item in a Menu.
 */
class MenuItem
{
private:
  std::string mName;
  SDL_Texture * mTexture;
public:
    MenuItem(const char * name);
    virtual ~MenuItem();

    /**
     * @brief This method is called whenever user interacts with the keyboard.
     * The notification is received only if the MenuItem is selected in a Menu.
     * @param key The key that was pressed.
     * @return If the method returns true, the parent Menu gets closed.
     */
    virtual bool keyEvent(int key) = 0;

    /**
     * @brief This method is called whenever the item is needed to be redrawn.
     * @param selected true if the item is selected by the parent Menu.
     * @return The drawn MenuItem.
     */
    virtual SDL_Texture * drawEventGUI() = 0;
    virtual std::string drawEventTUI(bool selected) = 0;

    const std::string & getName() const;

    /* SDL related */
    void updateTexture(const char * text);
    void render(int x, int y);
    SDL_Texture * getTexture();
};

/**
 * @brief The Button class serves as a pressable button with custom actions.
 */
class Button : public MenuItem
{
public:
    /**
     * @param name The caption of the Button.
     * @param action The function to be called when the Button gets pressed.
     */
    Button(const char * name, std::function<bool(void)> action);

    virtual ~Button();

    virtual bool keyEvent(int key);
    virtual std::string drawEventTUI(bool selected);
    virtual SDL_Texture * drawEventGUI();
private:
    std::function<bool(void)> mAction;
};

/**
 * @brief The List class provides a list of custom items one can choose from.
 */
class List : public MenuItem
{
public:
    /**
     * @param name The caption of the List.
     */
    List(const char * name);

    virtual ~List();

    virtual bool keyEvent(int key);
    virtual std::string drawEventTUI(bool selectd);
    virtual SDL_Texture * drawEventGUI();

    /**
     * @brief Adds an item to the list.
     * @param item Name of the new item.
     */
    void addItem(const char * item);

    /**
     * @brief Sets up default item that gets pre-selected.
     * @param id Id of the item. The ids start from zero and
     * get incremented by one whenever addItem() gets called.
     * @warning It is caller's responsibility to use a valid id.
     */
    void setDefaultItem(int id);

    /**
     * @brief Returns the name of the current item that is selected.
     * @return The name of the current item that is selected.
     */
    std::string curItem() const;
private:
    std::vector<std::string> mItems;
    int mPos;
};

/**
 * @brief The InputField class provides a way of prompting for user input.
 */
class InputField : public MenuItem
{
public:
    /**
     * @param name The caption of the InputField.
     * @param content The default content of the InputField.
     * @param limit The maximum number of characters user can input.
     */
    InputField(const char * name, const char * content, int limit);

    virtual ~InputField();

    virtual bool keyEvent(int key);
    virtual std::string drawEventTUI(bool selected);
    virtual SDL_Texture * drawEventGUI();

    /**
     * @brief Returns the content of the InputField.
     * @return The content of the InputField.
     */
    std::string content() const;
private:
    std::string mContent;
    int mLimit;
};

/**
 * @brief The Menu class represents a menu with items.
 */
class Menu
{
public:
    /**
     * @param name The caption of the Menu.
     * @param loopTill If the function's return value gets true while
     * the user interacts with the Menu, the Menu gets closed.
     */
    Menu(const char * name,
         std::function<bool(void)> * loopTill = 0);

    virtual ~Menu();

    /**
     * @brief This method is called whenever user interacts with the keyboard.
     * @param key The key that was pressed.
     */
    virtual void keyEvent(int key);

     /**
     * @brief This method is called whenever the Menu is needed to be redrawn.
     */
    virtual void drawEventTUI();
    virtual void drawEventGUI();

    /**
     * @brief Adds an MenuItem to the Menu.
     * @param item A new item to be added. The caller must allocate the item
     * on the heap. The Menu takes ownership of the new MenuItem and makes
     * sure the items get destroyed when it's needed.
     * @warning Caller is responsible for making sure there is enough
     * resources and space on the screen needed for a new item.
     */
    void addItem(MenuItem *item);
protected:
    /**
     * @brief Starts the Menu.
     */
    void loop();
private:
    std::vector<MenuItem*> mItems;
    int mPos;
    bool mExpired;
    std::string mName;

    std::function<bool(void)> * mLoopTill;
};

/**
 * @brief The Notification class represents a simple way of notifying user.
 */
class Notification : public Menu
{
public:
    /**
     * @param text The text of the Notification.
     * @param buttonText The caption of the confirmation button.
     * @param loopTill If the function's return value gets true while
     * the user interacts with the Notification, the Notification gets closed.
     */
    Notification(const char * text,
                 const char * buttonText = 0,
                 std::function<bool(void)> * loopTill = 0);

    virtual ~Notification() = default;
private:
    bool confirmAction();
};

}; // namespace UI

/*! @} End of Doxygen Groups*/

#endif
