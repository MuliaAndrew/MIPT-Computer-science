#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <sys/signal.h>


const char poison = '0';
int song_index = -1;
int wait_flg = 1;

void sigHandler(int signal);

bool is_busy(char* busy_letters, char letter); // check if letter is set as proc_letter in another process


int main(int argc, char** argv)
{
    int song_len = 0;

    for (int i = 1; i < argc; i++)
        song_len += strlen(argv[i]);

    char* song = (char*)calloc(song_len, sizeof(char));

    if (!song)
        exit(EXIT_FAILURE);

    for (int i = 1; i < argc; i++)
    {
        strcat(song, argv[i]);
    }

    sem_t * choosing_letter = (sem_t *)mmap(0, sizeof(*choosing_letter), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 , 0);

    if (choosing_letter == MAP_FAILED)
        exit(EXIT_FAILURE);
    
    if (sem_init(choosing_letter, 1, 1) == -1)
    {
        exit(EXIT_FAILURE);
    }

    int sh_id = shmget(IPC_PRIVATE, 34 * sizeof(char), IPC_CREAT | 0660);

    if (errno)
    {
        perror("in shmget()");
        return 1;
    }

    if (errno)
    {
        perror("in shmat()");
        return 1;
    }

    pid_t pid;

    for (int i = 0; i < 33; i++)
    {
        pid = fork();
        if (!pid)
            break;
    }

    signal(SIGUSR1, *sigHandler);

    int* busy_i = (int*)shmat(sh_id, 0, 0);
    *busy_i = 0;

    char* busy_letters = (char*)(busy_i + 1);


    if (!pid)
    {
        pid_t ch_pid = getpid();        

        if (errno)
        {
            perror("in getpid()");
            return 1;
        }

        char proc_letter = poison;

        while(song_index < song_len)
        {
            int sig;
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set, SIGUSR1);

            if (wait_flg)
                sigwait(&set, &sig);

            wait_flg = 1;

            if (proc_letter != poison)
            {
                if (song[song_index] == proc_letter)
                {
                    printf("[%d]: %c\n", ch_pid, proc_letter);
                    
                    kill(0, SIGUSR1);
                    raise(SIGUSR1);
                }
                else
                    continue;
            }

            else
            {
                if (is_busy(busy_letters, song[song_index]))
                    continue;
                else
                {
                    sem_wait(choosing_letter);

                    if (is_busy(busy_letters, song[song_index]))
                    {
                        sem_post(choosing_letter);
                        continue;
                    }
                    else
                    {
                        proc_letter = song[song_index];
                        busy_letters[*busy_i++] = proc_letter;

                        sem_post(choosing_letter);
                        
                        printf("[%d]: %c, char %d\n", ch_pid, proc_letter, proc_letter);
                        
                        kill(0, SIGUSR1);
                        raise(SIGUSR1);
                    }
                }
            }
        }
    }

    else
    {
        kill(0, SIGUSR1);
        int wstatus;
        for (int i = 0; i < 33; i++)
            wait(&wstatus);
        
        exit(EXIT_SUCCESS);
    }
}

bool is_busy(char* busy_letters, char letter)
{
    int i = 0;
    for (; busy_letters[i] != 0; i++)
    {
        if (letter == busy_letters[i])
            return 1;
    }
    
    if (i > 25)
        return 1;

    return 0;
}

void sigHandler(int signal)
{
    song_index++;
    wait_flg = 0;
};