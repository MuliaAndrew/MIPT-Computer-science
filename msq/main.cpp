#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


void judge();

void runner(int index);

int n_runners;
int id;


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Need 1 argument: integer number\n");
        return 1;
    }

    n_runners = atoi(argv[1]);
    
    id = msgget(IPC_PRIVATE, IPC_CREAT | 0777);

    pid_t pid;
    
    int i = 1;
    for(; i <= n_runners; i++)
    {
        pid = fork();

        if (pid)
            continue;
        else
            break;
    }

    if (pid)
        judge();
    else
        runner(i);

    return 0;
}


void judge()
{
    msgbuf msg = {};
    
    printf("Judge came on stadium\n");
    printf("Judge waiting for all members...\n");
    
    /// Confirming all are present
    for (int i = 1; i <= n_runners; i++)
    {
        msgrcv(id, &msg, 0, 0, 0);
    }

    printf("Judge confirms all members have came\n");

    msg.mtype = n_runners + 1;
    for (int i = 1; i <= n_runners; i++)
        msgsnd(id, &msg, 0, 0);

    printf("Judge start run\n");

    /// Send msg to first to start
    clock_t st = clock();
    msg.mtype = n_runners + 10;
    msgsnd(id, &msg, 0, 0);
    
    /// Waiting the last runner sends finish msg
    msgrcv(id, &msg, 0, n_runners, 0);
    clock_t end = clock();

    printf("Judge: run time: %ld us.\n", end - st ); // print run time
}


void runner(int index)
{
    msgbuf msg = {};
    
    printf("Runner #%d has come\n", index);
    
    /// tell judge that runner has come
    msg.mtype = (long)index;
    msgsnd(id, &msg, 0, 0);

    msgrcv(id, &msg, 0, n_runners + 1, 0);

    /// waiting judge command to start run or previous runner told that has finished
    if (index == 1)
        msgrcv(id, &msg, 0, n_runners + 10, 0);
    else
        msgrcv(id, &msg, 0, index - 1, 0);

    printf("Runner #%d start running\n", index);

    usleep(50 * index);
    
    printf("Runner #%d finished\n", index);

    /// Tell Runner index + 1 to start running
    msg.mtype = index;
    msgsnd(id, &msg, 0, 0);
}