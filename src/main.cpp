#include "mainmenu.h"

int main(int argc, char ** argv)
{
    UI::init();
    MainMenu menu(argv[0]);
    UI::deinit();
    return 0;
}
