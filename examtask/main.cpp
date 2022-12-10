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


const char poison = '0';


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

    int sh_id = shmget(IPC_PRIVATE, song_len + 2 * sizeof(int), IPC_CREAT | 0660);

    if (errno)
    {
        perror("in shmget()");
        return 1;
    }

    int* txt_index = (int*)shmat(sh_id, 0 , 0);

    if (errno)
    {
        perror("in shmat()");
        return 1;
    }

    int* n_remaining = txt_index + 1;

    char* letter_for_choosing = (char*)(n_remaining + 1);

    *txt_index = 0;
    *n_remaining = 27;

    strcpy(letter_for_choosing, "abcdefghijklmnopqrstuvwxyz");

    pid_t pid;

    printf("text: %s\n", song);

    printf("letters: %s\n", letter_for_choosing);

    for (int i = 0; i < 27; i++)
    {
        pid = fork();
        if (!pid)
            break;
    }

    if (!pid)
    {
        pid_t ch_pid = getpid();

        if (errno)
        {
            perror("in getpid()");
            return 1;
        }

        int* txt_index = (int*)shmat(sh_id, 0, 0);

        if (errno)
        {
            perror("in shmat()");
            return 1;
        }

        int* n_remaining = txt_index + 1;
        
        char* remaining_letters = (char*)(n_remaining + 1);

        // printf("remaining letters: %s\n", remaining_letters);

        //Choosing letter for every proc

        sem_wait(choosing_letter);

        int i = 0;

        // looking for available letter in arr
        for (; i < 27; i++)
        {
            if (remaining_letters[i] != poison)
                break;
        }

        if (i >= 27)
        {
            perror("wtf");
            exit(EXIT_SUCCESS);
        }
        char proc_letter = remaining_letters[i];

        remaining_letters[i] = poison; // make letter unavailable
        
        *n_remaining--; // here is used to count how many processes didnt choosed letter

        sem_post(choosing_letter);

        while(*txt_index < song_len)
        {
            if (song[*txt_index] == proc_letter)
            printf("[%u : index = %d]: %c", ch_pid, *txt_index, proc_letter);
            *txt_index++;
        }
    }

    else
    {
        int wstatus;
        for (int i = 0; i < 27; i++)
            wait(&wstatus);
        
        exit(EXIT_SUCCESS);
    }
}