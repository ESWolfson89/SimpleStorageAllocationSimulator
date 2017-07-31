#include "button.h"

button::button()
{
}

button::button(int lx, int ly, int sx, int sy)
{
  locx = lx;
  locy = ly;
  sizex = sx;
  sizey = sy;
}

int button::getLocX()
{
  return locx;
}

int button::getLocY()
{
  return locy;
}

int button::getSizeX()
{
  return sizex;
}

int button::getSizeY()
{
  return sizey;
}

bool button::checkClick(int px, int py)
{
  if (py >= locy && py < locy + sizey &&
      px >= locx && px < locx + sizex)
  {
      return true;
  }
  return false;
}
