#include "bomberman.h"

#include <string.h>

int main(int argc, char ** argv)
{
  bool gui = false;

  if ( argc == 2 &&
       strcmp(argv[1], "--gui") == 0 )
    gui = true;

  Bomberman b(argv[0], gui);
  return 0;
}
