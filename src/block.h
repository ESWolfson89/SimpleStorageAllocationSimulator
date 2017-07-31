#ifndef BLOCK_H
#define BLOCK_H

#include "curses.h"
#include <vector>

static const int SCREEN_WID = 100;
static const int SCREEN_HGT = 42;

#define MAX_BLOCK_SIZE 512
#define MIN_BLOCK_SIZE 1

class RAM_partition
{
    public:
        RAM_partition();
        RAM_partition(int,int,int,chtype);
        void incBlockSize(int);
        void setColorIndex(int);
        void setProcID(int);
        void setOffset(int);
        int getBlockSize();
        chtype getColorIndex();
        int getProcID();
        int getOffset();
    private:
        chtype color_index;
        int block_size;
        int proc_id; // -1 means empty
        int offset;
};

#endif
