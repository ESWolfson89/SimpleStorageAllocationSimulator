#ifndef MEMMANAGER_H
#define MEMMANAGER_H

#include "block.h"
#include <stdlib.h>
#include <time.h>

#define MIN_NUM_BLOCKS 1
#define MAX_NUM_BLOCKS 512
#define MAX_NUM_PROCESSES 100

#define TEXT_BLACK 1
#define TEXT_WHITE 2
#define TEXT_PURPLE 3
#define TEXT_RED 4
#define TEXT_GREEN 5
#define TEXT_BLUE 6
#define TEXT_YELLOW 7
#define TEXT_CYAN 8

static const chtype color_array[8] =
{
    COLOR_PAIR(1) | A_BOLD,
    COLOR_PAIR(2) | A_DIM,
    COLOR_PAIR(3) | A_DIM,
    COLOR_PAIR(4) | A_DIM,
    COLOR_PAIR(5) | A_DIM,
    COLOR_PAIR(6) | A_DIM,
    COLOR_PAIR(7) | A_DIM,
    COLOR_PAIR(8) | A_DIM
};

enum scheme
{
      FIRSTFIT,
      BESTFIT,
      WORSTFIT
};

enum schedule
{
      FIFO,
      LIFO
};

struct proc
{
    int proc_size;
    int proc_id;
    int proc_lifetime;
    int proc_timer;
    chtype proc_color;
    bool is_allocated;
};

class mem_manager
{
    public:
        mem_manager();
        void init();
        void executeOneStorageIteration();
        void executeOneReleaseIteration();
        void resetStats();
        void resetManager();
        void execFirstFit(proc *);
        void execBestFit(proc *);
        void execWorstFit(proc *);
        void storeBlock(int,proc *);
        void releaseBlock(int,proc *);
        void setInputProcSize(int, int);
        void setNextAlg();
        void eraseBlock(int);
        void addBlock(int,int,int,int,chtype);
        void addInpProc(int);
        void eraseInpProc(int);
        void updateProcIDs();
        void checkProcTimers();
        proc *getInpProc(int);
        RAM_partition *getBlock(int);
        scheme getCurrentAlg();
        int getProcListSize();
        int getNumBlocks();
        int getMemorySize();
        int getMemoryUsage();
        int getProcessesStored();
        int getNextProcID();
        int getNumMostRecentlyReleasedProcessIDs();
        int getMostRecentlyReleasedProcessID(int);
        int getMostRecentlyStoredBlockID();
    private:
        std::vector<RAM_partition> main_memory;
        std::vector<proc> proc_list;
        std::vector<int> most_recently_released_process_ids;
        int most_recently_stored_block_id;
        int num_processes;
        int memory_used;
        int total_memory;
        int processes_used;
        int next_proc_id;
        schedule schedule_alg;
        scheme scheme_alg;
};

#endif
