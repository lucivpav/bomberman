#include "bomberman.h"

#include "mainmenu.h"

#include <ctime>
#include <sys/signal.h>

Bomberman::Bomberman(const char *argv0, bool gui)
{
  /* prepare pseudo random number generator */
  srand(time(0));

  /* disable SIGPIPE when sending to an invalid socket */
  signal(SIGPIPE, SIG_IGN);

  UI ui(gui);
  MainMenu m(argv0);
}
