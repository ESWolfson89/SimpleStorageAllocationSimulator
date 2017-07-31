#include "mmanager.h"

mem_manager::mem_manager()
{
    total_memory = MAX_BLOCK_SIZE;
}

RAM_partition *mem_manager::getBlock(int b)
{
    return &main_memory[b];
}

proc *mem_manager::getInpProc(int i)
{
    return &proc_list[i];
}

int mem_manager::getNextProcID()
{
    return next_proc_id;
}

int mem_manager::getNumBlocks()
{
    return (int)main_memory.size();
}

int mem_manager::getProcListSize()
{
    return (int)proc_list.size();
}

void mem_manager::setInputProcSize(int i, int p_size)
{
    proc_list[i].proc_size = p_size;
}

void mem_manager::addInpProc(int i)
{
    proc p = proc_list[i];
    proc_list.insert(proc_list.begin()+i,p);
    updateProcIDs();
}

void mem_manager::updateProcIDs()
{
    for (int j = 0; j < (int)proc_list.size(); ++j)
    {
        proc_list[j].proc_id = j;
    }
}

void mem_manager::eraseInpProc(int i)
{
    proc_list.erase(proc_list.begin()+i);
    updateProcIDs();
}

void mem_manager::eraseBlock(int i)
{
    main_memory.erase(main_memory.begin()+i);
}

void mem_manager::setNextAlg()
{
    if (scheme_alg == WORSTFIT)
        scheme_alg = FIRSTFIT;
    else
        scheme_alg = (scheme)((int)scheme_alg + 1);
}

void mem_manager::resetStats()
{
    //total_memory = MAX_BLOCK_SIZE;
}

void mem_manager::resetManager()
{
    processes_used = 0;
    memory_used = 0;
}

void mem_manager::addBlock(int i, int block_size,int pid,int offset,chtype color_index)
{
    RAM_partition rp = RAM_partition(block_size,pid,offset,color_index);
    main_memory.insert(main_memory.begin()+i,rp);
}

void mem_manager::init()
{
    scheme_alg = FIRSTFIT;
    schedule_alg = FIFO;

    next_proc_id = 0;
    memory_used = 0;

    proc p;

    main_memory.push_back(RAM_partition(MAX_BLOCK_SIZE,-1,0,COLOR_PAIR(TEXT_BLACK)|A_DIM));

    for (int i = 0; i < MAX_NUM_PROCESSES; ++i)
    {
        p.proc_id = i;
        p.proc_size = (rand() % 30) + 1;
        p.proc_lifetime = (rand() % 38) + 3;
        p.proc_timer = 0;
        p.is_allocated = false;
        p.proc_color = color_array[i%8];
        proc_list.push_back(p);
    }

    num_processes = (int)proc_list.size();
    processes_used = 0;

    //setupUpperLimitFields();
}

scheme mem_manager::getCurrentAlg()
{
    return scheme_alg;
}

int mem_manager::getMemorySize()
{
    return total_memory;
}

int mem_manager::getMemoryUsage()
{
    return memory_used;
}

int mem_manager::getProcessesStored()
{
    return processes_used;
}

int mem_manager::getNumMostRecentlyReleasedProcessIDs()
{
    return (int)most_recently_released_process_ids.size();
}

int mem_manager::getMostRecentlyReleasedProcessID(int i)
{
    return most_recently_released_process_ids[i];
}

int mem_manager::getMostRecentlyStoredBlockID()
{
    return most_recently_stored_block_id;
}

void mem_manager::executeOneStorageIteration()
{
    most_recently_stored_block_id = -1;
    if (processes_used < num_processes) {
        while(proc_list[next_proc_id].is_allocated)
        {
            next_proc_id = (next_proc_id + 1) % num_processes;
        }
        switch(scheme_alg)
        {
            case(FIRSTFIT):
                 execFirstFit(&proc_list[next_proc_id]);
                 break;
            case(BESTFIT):
                 execBestFit(&proc_list[next_proc_id]);
                 break;
            case(WORSTFIT):
                 execWorstFit(&proc_list[next_proc_id]);
                 break;
            default:
                break;
        }
    }
}

void mem_manager::checkProcTimers()
{
    for (int i = 0; i < (int)proc_list.size(); ++i)
    {
        if (proc_list[i].is_allocated)
            proc_list[i].proc_timer++;
        if (proc_list[i].proc_timer == proc_list[i].proc_lifetime)
        {
            proc_list[i].proc_timer = 0;
            most_recently_released_process_ids.push_back(i);
        }
    }
}

void mem_manager::executeOneReleaseIteration()
{
    int proc_index;
    for (int p = 0; p < (int)most_recently_released_process_ids.size(); ++p)
    {
        proc_index = most_recently_released_process_ids[p];
        for (int m = 0; m < (int)main_memory.size(); ++m)
        {
            if (proc_index == main_memory[m].getProcID())
            {
                releaseBlock(m,&proc_list[proc_index]);
                break;
            }
        }
    }
    most_recently_released_process_ids.clear();
}

void mem_manager::execFirstFit(proc *p)
{
    int chunk_size;

    for (int i = 0; i < (int)main_memory.size(); ++i)
    {
        if (main_memory[i].getProcID() == -1)
        {
            chunk_size = main_memory[i].getBlockSize();

            if (chunk_size >= p->proc_size)
            {
                storeBlock(i,p);
                return;
            }
        }
    }
}

void mem_manager::execBestFit(proc *p)
{
    int index = 0;

    int min_val = MAX_BLOCK_SIZE + 1;

    int space_left;

    for (int i = 0; i < (int)main_memory.size(); ++i)
    {
        if (main_memory[i].getProcID() == -1)
        {
            if (main_memory[i].getBlockSize() >= p->proc_size)
            {
                space_left = main_memory[i].getBlockSize() - p->proc_size;
                if (space_left < min_val)
                {
                    min_val = space_left;
                    index = i;
                }
            }
        }
    }

    if (min_val <= MAX_BLOCK_SIZE)
    {
        storeBlock(index,p);
    }
}

void mem_manager::execWorstFit(proc *p)
{
    int index = 0;

    int max_val = -1;

    int space_left;

    for (int i = 0; i < (int)main_memory.size(); ++i)
    {
        if (main_memory[i].getProcID() == -1)
        {
            if (main_memory[i].getBlockSize() >= p->proc_size)
            {
                space_left = main_memory[i].getBlockSize() - p->proc_size;

                if (space_left > max_val)
                {
                    max_val = space_left;
                    index = i;
                }
            }
        }
    }

    if (max_val > -1)
    {
        storeBlock(index,p);
    }
}

void mem_manager::storeBlock(int index, proc *p)
{
    if (p->proc_size == main_memory[index].getBlockSize())
    {
        main_memory[index].setProcID(p->proc_id);
        main_memory[index].setColorIndex(p->proc_color);
    }
    else
    {
        addBlock(index,p->proc_size,p->proc_id,main_memory[index].getOffset(),p->proc_color);
        main_memory[index+1].setOffset(main_memory[index].getOffset()+p->proc_size);
        main_memory[index+1].incBlockSize(-1*p->proc_size);
    }
    p->is_allocated = true;
    memory_used += p->proc_size;
    processes_used++;
    most_recently_stored_block_id = index;
}

// make into switch statement
void mem_manager::releaseBlock(int index, proc *p)
{
    bool top_block = (index == 0);
    bool bottom_block = (index == (int)main_memory.size() - 1);
    bool middle_block = (index < (int)main_memory.size() - 1 && index > 0);

    if (middle_block) {
         if (main_memory[index+1].getProcID() == -1 && main_memory[index-1].getProcID() == -1)
         {
             main_memory[index-1].incBlockSize(main_memory[index].getBlockSize()+main_memory[index+1].getBlockSize());
             main_memory.erase(main_memory.begin()+index);
             main_memory.erase(main_memory.begin()+index);
         }
         else if (main_memory[index+1].getProcID() > -1 && main_memory[index-1].getProcID() > -1)
         {
             main_memory[index].setColorIndex(1);
             main_memory[index].setProcID(-1);
         }
         else if (main_memory[index+1].getProcID() == -1) {
             main_memory[index+1].setOffset(main_memory[index].getOffset());
             main_memory[index+1].incBlockSize(main_memory[index].getBlockSize());
             main_memory.erase(main_memory.begin()+index);
         }
         else if (main_memory[index-1].getProcID() == -1) {
             main_memory[index-1].incBlockSize(main_memory[index].getBlockSize());
             main_memory.erase(main_memory.begin()+index);
         }
    }
    else if (bottom_block) {
         if (main_memory[index-1].getProcID() > -1) {
             main_memory[index].setColorIndex(1);
             main_memory[index].setProcID(-1);
         }
         else {
             main_memory[index-1].incBlockSize(main_memory[index].getBlockSize());
             main_memory.erase(main_memory.begin()+index);
         }
    }
    else if (top_block) {
         if (main_memory[index+1].getProcID() > -1) {
             main_memory[index].setColorIndex(1);
             main_memory[index].setProcID(-1);
         }
         else {
             main_memory[index+1].setOffset(main_memory[index].getOffset());
             main_memory[index+1].incBlockSize(main_memory[index].getBlockSize());
             main_memory.erase(main_memory.begin()+index);
         }
    }

    p->is_allocated = false;
    memory_used -= p->proc_size;
    processes_used--;
}
