#include "block.h"

RAM_partition::RAM_partition()
{
    block_size = MAX_BLOCK_SIZE;
}

RAM_partition::RAM_partition(int s, int pid, int off, chtype col)
{
    block_size = s;
    color_index = col;
    proc_id = pid;
    offset = off;
}

int RAM_partition::getBlockSize()
{
    return block_size;
}

chtype RAM_partition::getColorIndex()
{
    return color_index;
}

int RAM_partition::getProcID()
{
    return proc_id;
}

int RAM_partition::getOffset()
{
    return offset;
}

void RAM_partition::incBlockSize(int inc)
{
    block_size += inc;
}

void RAM_partition::setColorIndex(int ci)
{
    color_index = ci;
}

void RAM_partition::setProcID(int pid)
{
    proc_id = pid;
}

void RAM_partition::setOffset(int off)
{
    offset = off;
}
