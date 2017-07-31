#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "mmanager.h"
#include "button.h"
#include <string>
#include <sstream>

class sim
{
    public:
        sim();
        void runProg();
        void reDisplayProcData();
        void delayFunc(int);
        void initDisplay();
        void initButtons();
        void runSimulation();
        void checkRunSimButton();
        void checkAlgButton();
        void checkQuitButton();
        void printText(int,int,std::string,int,chtype);
        void printNumber(int,int,int,int,chtype);
        void printInputProc(int,int, proc, bool);
        void printPartitionBar(RAM_partition *,bool);
        void printButtons();
        void clearQueueDataDisplay();
        void clearPartitionDataDisplay();
        void clearPartitionContentsDisplay();
        void addChar(int,int,chtype);
        void checkSimPausedButtons();
        void checkSimRunningButtons();
        void checkProcBarScroll();
        void checkInputQueueButtons();
        void promptModifyBlockSize(int);
        void promptModifyInputProcSize(int);
        void displayPartitionContents(int);
        void displayStorage();
        void displayReleases();
        void clearRightSide();
        std::string promptNumber(int,int);
    private:
        mem_manager mmanage_obj;
        MEVENT minput;
        button alg_button;
        button procbar_scroll_area;
        button runsim_button;
        button runprog_button;
        int procbar_offset;
        int blockbar_offset;
        int ticks;
        bool simulation_running;
        bool program_running;
};

int string2int(std::string);
std::string int2String(int);

#endif // SIMULATOR_H
