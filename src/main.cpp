#include "simulator.h"

void initColor();
void initPDcurses();

int main(int argc, char* argv[])
{
    initPDcurses();

    sim sim_obj;

    sim_obj.runProg();

    endwin();

    return 0;
}

void initColor()
{
    start_color();
    init_pair(1,COLOR_BLACK,COLOR_BLACK);
    init_pair(2,COLOR_WHITE,COLOR_BLACK);
    init_pair(3,COLOR_MAGENTA,COLOR_BLACK);
    init_pair(4,COLOR_RED,COLOR_BLACK);
    init_pair(5,COLOR_GREEN,COLOR_BLACK);
    init_pair(6,COLOR_BLUE,COLOR_BLACK);
    init_pair(7,COLOR_YELLOW,COLOR_BLACK);
    init_pair(8,COLOR_CYAN,COLOR_BLACK);
}

// initialize pdcurses
void initPDcurses()
{
    srand(time(NULL));
    initscr();
    resize_term(SCREEN_HGT,SCREEN_WID);
    initColor();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr,TRUE);
    mousemask(BUTTON1_CLICKED | BUTTON1_PRESSED,NULL);
    PDC_set_title("Simple Storage Allocation Simulator");
}
