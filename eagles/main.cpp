#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>


int main(int argc, char** argv)
{
    const int N = 20;   // Amount of food that mother eagle brings

    int sh_id = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | 0660);

    int * start_n = (int *)shmat(sh_id, 0, 0);
    *start_n = N;

    const int n_eagles = 6;

    int n_iters = 10;

    sem_t * bowl_block = (sem_t *)mmap(0, sizeof(*bowl_block), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_t * call_mom = (sem_t *)mmap(0, sizeof(*bowl_block), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (bowl_block == MAP_FAILED || call_mom == MAP_FAILED)
    {
        perror("in mmap()");
        return 0;
    }
    
    if (sem_init(bowl_block, 1, 1) == -1)
    {
        perror("in sem_init()");
        return 1;
    }

    if (sem_init(call_mom, 1, 0) == -1)
    {
        perror("in sem_init()");
        return 1;
    }

    pid_t pid;

    int i = 0;
    for (; i < n_eagles; i++)
    {
        pid = fork();
        if (pid)
            break;
    }

    if (pid)
    {
        int * n = (int *)shmat(sh_id, 0, 0); // Current amount of food in bowl

        if (errno)
        {
            perror("in shmat()");
            return 1;
        }

        for (int iter = 0; iter < n_iters; iter++)
        {
            printf("[EAGLE %d] waked up and want to eat\n", i);

            sem_wait(bowl_block);

            (*n)--;

            printf("[EAGLE %d] has eaten and going to sleep \t n = %d\n", i, *n);

            if ((*n) > 0)
                sem_post(bowl_block);
            else
            {
                printf("[EAGLE %d] call mother to bring food\n", i);
                sem_post(call_mom);
            }
            sleep(1);
        }
    }

    else
    {
        int * n = (int *)shmat(sh_id, 0, 0); // Current amount of food in bowl

        if (errno)
        {
            perror("in shmat()");
            return 1;
        }

        while (1)
        {
            sem_wait(call_mom);
            *n = N;
            printf("[MOTHER] brings food \t n = %d\n", *n);
            sem_post(bowl_block);
        }
    }

    return 0;
}