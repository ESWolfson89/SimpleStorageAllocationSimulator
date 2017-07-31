#include "simulator.h"

sim::sim()
{
    ticks = 0;
    procbar_offset = 0;
    simulation_running = false;
    program_running = true;
}

void sim::initButtons()
{
    procbar_scroll_area = button(81,1,19,33);
    runsim_button = button(SCREEN_WID-10,34,9,1);
    alg_button = button(SCREEN_WID-10,37,9,1);
    runprog_button = button(SCREEN_WID-10,40,4,1);
}

void sim::delayFunc(int ms)
{
    napms(ms);
    refresh();
}

void sim::runProg()
{
    clear();
    mmanage_obj.init();
    initButtons();
    initDisplay();
    reDisplayProcData();

    do
    {
        checkSimPausedButtons();

        while (simulation_running)
        {
            runSimulation();
        }

    } while (program_running);
}

void sim::runSimulation()
{
    mmanage_obj.executeOneStorageIteration();
    reDisplayProcData();
    displayStorage();
    delayFunc(400);
    checkSimRunningButtons();
    mmanage_obj.checkProcTimers();
    reDisplayProcData();
    displayReleases();
    mmanage_obj.executeOneReleaseIteration();
    ticks++;
}

void sim::checkSimRunningButtons()
{
    int keyp = getch();
    nc_getmouse(&minput);
    if (keyp == KEY_MOUSE)
    {
        checkRunSimButton();
    }
}

void sim::checkRunSimButton()
{
    if (runsim_button.checkClick(minput.x,minput.y))
    {
        simulation_running = !simulation_running;

        if (simulation_running)
            timeout(0);

        printButtons();
    }
}

void sim::clearRightSide()
{
    for (int x = 81; x < SCREEN_WID; ++x)
    {
        for (int y = 0; y < 34; ++y)
        {
            addChar(x,y,' ');
        }
    }
}

void sim::initDisplay()
{
    printText(0,33,"---------------------------------------------------------------------------------",2,A_BOLD);
    printText(0,0, "---------------------------------------------------------------------------------",2,A_BOLD);
    printText(1,36,"Statistics and other input options will be added in this section later on.",2,A_DIM);
    printText(1,37,"For now, the number of input processes is held static at 100.",2,A_DIM);
    printText(1,38,"Process lifetime range: (3-40 ticks) Process size range: (3-30 KB).",2,A_DIM);
    printText(1,39,"Use the buttons on the right for interaction.",2,A_DIM);
    printText(1,40,"Click the panel on the right to scroll through processes.",2,A_DIM);
    for (int i = 0; i < 17; ++i)
    {
        if (i < 16)
            printText((i*5)+1,34,int2String((i+1)*32) + "K",2,A_BOLD);
        for (int y = 1; y <= 32; ++y)
        {
            addChar((i*5),y,'|'|COLOR_PAIR(2)|A_BOLD);
        }
    }

    printButtons();
}

void sim::reDisplayProcData()
{
    clearRightSide();
    int x_off;
    for (int i = 0; i < 32; ++i)
    {
        x_off = i + procbar_offset;
        if (i < mmanage_obj.getProcListSize())
        {
            printText(83,i+1,"                ",2,mmanage_obj.getInpProc(x_off)->is_allocated ? (A_DIM|A_REVERSE) : A_DIM);
            printText(83,i+1,"P" + int2String(mmanage_obj.getInpProc(x_off)->proc_id),2,mmanage_obj.getInpProc(x_off)->is_allocated ? (A_DIM|A_REVERSE) : A_DIM);
            printText(88,i+1,int2String(mmanage_obj.getInpProc(x_off)->proc_size) + "K",2,mmanage_obj.getInpProc(x_off)->is_allocated ? (A_DIM|A_REVERSE) : A_DIM);
            printText(93,i+1,int2String(mmanage_obj.getInpProc(x_off)->proc_timer) + "/" + int2String(mmanage_obj.getInpProc(x_off)->proc_lifetime),2,mmanage_obj.getInpProc(x_off)->is_allocated ? (A_DIM|A_REVERSE) : A_DIM);
            if (mmanage_obj.getInpProc(x_off)->proc_id == mmanage_obj.getNextProcID())
            {
                addChar(82,i + 1,'+'|COLOR_PAIR(TEXT_PURPLE)|A_BOLD);
            }
            for (int p = 0; p < mmanage_obj.getNumMostRecentlyReleasedProcessIDs(); ++p)
            {
                if (mmanage_obj.getMostRecentlyReleasedProcessID(p) == mmanage_obj.getInpProc(x_off)->proc_id)
                {
                    addChar(82,i + 1,'-'|COLOR_PAIR(TEXT_YELLOW)|A_BOLD);
                    break;
                }
            }
        }
    }
    refresh();
}

// it is more natural to specify x parameter before y
void sim::printText(int x, int y, std::string text, int color, chtype mask)
{
    attron(COLOR_PAIR(color)|mask);
    mvprintw(y,x,&text[0]);
    attroff(COLOR_PAIR(color)|mask);
}

// it is more natural to specify x parameter before y
void sim::printNumber(int x, int y, int number, int color, chtype mask)
{
    attron(COLOR_PAIR(color)|mask);
    mvprintw(y,x,"%u",number);
    attroff(COLOR_PAIR(color)|mask);
}

void sim::displayStorage()
{
    int index = mmanage_obj.getMostRecentlyStoredBlockID();
    if (index >= 0)
        printPartitionBar(mmanage_obj.getBlock(index),true);
}

void sim::displayReleases()
{
    int proc_id;
    for (int i = 0; i < mmanage_obj.getNumMostRecentlyReleasedProcessIDs(); ++i)
    {
        proc_id = mmanage_obj.getMostRecentlyReleasedProcessID(i);
        for (int m = 0; m < mmanage_obj.getNumBlocks(); ++m)
        {
            if (mmanage_obj.getBlock(m)->getProcID() == proc_id)
            {
                printPartitionBar(mmanage_obj.getBlock(m),false);
                delayFunc(400);
                checkSimRunningButtons();
                break;
            }
        }
    }
}

void sim::printPartitionBar(RAM_partition *rp,bool add_block)
{
    int y_val = (rp->getOffset() % 32);
    int x_val = (rp->getOffset() / 32) * 5;
    int x_start = x_val;
    int y_start = y_val;
    int loop_iters = 0;

    int max_y = y_val + rp->getBlockSize();

    for (int y = y_val; y < max_y; ++y) {
         //delay and display animation
         if (y > 0 && y % 32 == 0) {
             x_val += 5;
             max_y -= 32;
             y = 0;
         }
         for (int x = 0; x < 4; ++x)
              addChar(x_val+x+1,y+1,add_block ? (219|rp->getColorIndex()) : ' ');
         if (loop_iters == 0 && add_block)
             printText(x_start+1,y_start+1,"P" + int2String(rp->getProcID()),2,A_DIM|A_REVERSE);
         delayFunc(30);
         loop_iters++;
    }
}

void sim::printButtons()
{
    if (!simulation_running)
        printText(runsim_button.getLocX(),runsim_button.getLocY()," RUN ",TEXT_GREEN,A_DIM|A_REVERSE);
    else
        printText(runsim_button.getLocX(),runsim_button.getLocY(),"PAUSE",TEXT_RED,A_DIM|A_REVERSE);
    printText(runprog_button.getLocX(),runprog_button.getLocY(),"QUIT",TEXT_BLUE,A_DIM|A_REVERSE);
    switch(mmanage_obj.getCurrentAlg())
    {
        case(FIRSTFIT):
            printText(alg_button.getLocX(),alg_button.getLocY(),"First Fit",TEXT_WHITE,A_DIM|A_REVERSE);
            break;
        case(BESTFIT):
            printText(alg_button.getLocX(),alg_button.getLocY(),"Best Fit ",TEXT_WHITE,A_DIM|A_REVERSE);
            break;
        case(WORSTFIT):
            printText(alg_button.getLocX(),alg_button.getLocY(),"Worst Fit",TEXT_WHITE,A_DIM|A_REVERSE);
            break;
    }
}

void sim::addChar(int x, int y, chtype ct)
{
    mvdelch(y,x);
    mvinsch(y,x,ct);
}

void sim::checkProcBarScroll()
{
    if (procbar_scroll_area.checkClick(minput.x,minput.y))
    {
        procbar_offset += (minput.y - (SCREEN_HGT/2));

        if (minput.y >= (SCREEN_HGT/2))
            procbar_offset++;

        if (procbar_offset < 0 || mmanage_obj.getProcListSize() <= 32)
            procbar_offset = 0;
        else if (procbar_offset > mmanage_obj.getProcListSize() - 32)
            procbar_offset = (mmanage_obj.getProcListSize() - 32);

        reDisplayProcData();
    }
}

void sim::checkQuitButton()
{
    if (runprog_button.checkClick(minput.x,minput.y))
    {
        program_running = false;
    }
}

void sim::checkAlgButton()
{
    if (alg_button.checkClick(minput.x,minput.y))
    {
        mmanage_obj.setNextAlg();
        printButtons();
    }
}

void sim::checkSimPausedButtons()
{
    timeout(-1);
    int keyp = getch();
    nc_getmouse(&minput);
    if (keyp == KEY_MOUSE)
    {
        checkProcBarScroll();
        checkRunSimButton();
        checkAlgButton();
        checkQuitButton();
    }
}

void sim::promptModifyInputProcSize(int i)
{
    std::string s = promptNumber(44,9+i);

    int proc_size = 1;

    if (s == "")
        proc_size = 128;
    else
    {
        proc_size = string2int(s);
        if (proc_size < 1 || proc_size > MAX_BLOCK_SIZE)
            proc_size = 128;
    }

    mmanage_obj.setInputProcSize(i,proc_size);
}

std::string sim::promptNumber(int x_off, int y_off)
{
    int keyp;
    int position = 1;

    printText(x_off+1,y_off,"     ",TEXT_BLACK,A_DIM);
    std::string s = "";

    addChar(position+x_off,y_off,' ');
    curs_set(1);
    char c;
    // wait for enter key
    while((keyp = getch()) != 10)
    {
       if ((keyp >= (int)'0' && keyp <= '9') &&
           position <= 5)
       {
         c = (char)(keyp);
         s += c;
         addChar(position+x_off,y_off,c);
         position++;
       }
       else if (keyp == 8 && position > 1)
       {
          position--;
          s.erase(s.begin()+position-1,s.end());
          addChar(position+x_off,y_off,' ');
       }
       move(y_off,position+x_off);
    }

    curs_set(0);

    return s;
}

int string2int(std::string s)
{
    int result = 0;
    int digit = 0;
    int factor = 1;
    int ssize = s.size();
    for (int i = ssize-1; i >= 0; --i)
    {
        digit = (int)s[ssize-i-1] - 48;
        factor = 1;
        for (int j = 0; j < i; ++j)
        {
            factor *= 10;
        }
        result += (digit * factor);
    }
    return (int)result;
}

std::string int2String(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}
