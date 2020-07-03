#include "ncurses_display.h"
#include "system.h"
#include <iostream>
#include "format.h"

using std::cout;

int main() {
  System system;
  NCursesDisplay::Display(system);
}
