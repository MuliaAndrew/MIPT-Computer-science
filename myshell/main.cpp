#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>


#define READ_PIPE 0    // in cur_pipe refers to read fd
#define WRITE_PIPE 1   // --||-- to write fd


const char  EXIT_CMD[]      = "exit";
const int   CMD_MAX_LEN     = 1024;
const int   WORDS_ARR_SIZE  = 512;
const char  PROMT[]         = " >> ";
const char  CONVEYER        = '|';


enum Condition
{
    SPACE,
    NOT_SPACE,
};

/*
 In readed user command rewrite all space symbols to \0
 Used to divide command to several words
 Returns the amount of words
*/
int rebuild_cmd(char* cmd, char** word);

int main()
{
    while(1)
    {
        int status;
        while (errno != ECHILD)
            wait(&status);
        
        write(STDOUT_FILENO, (char*)PROMT, 5);

        char cmd[CMD_MAX_LEN] = {};

        int readBytes = read(STDIN_FILENO, (char*)cmd, CMD_MAX_LEN);
        if (readBytes == CMD_MAX_LEN)
        {
            fprintf(stderr, "Command length must be lower the 1024 symbols");
            return -1;
        }
        if (readBytes == -1)
        {
            perror("read from stdin");
            _exit(1);
        }
        if (!readBytes)
            return 0;

        char* word[WORDS_ARR_SIZE] = {}; // constains the pointers on words in cmd string

        int n_words = rebuild_cmd(cmd, word);

        if (!strcmp(word[0], EXIT_CMD))
            return 0;

        int source_offset = 0;
        int dest_offset = 0;
        int in_fd_prev = -1; // Input fd of pipe where previous command will write output
        int in_fd_next = -1; // Saved input fd of pipe for next command where it will write output
        int cur_pipe[2];

    }
}

int rebuild_cmd(char* cmd, char** word)
{
    int cond    = NOT_SPACE;
    int word_i  = 0;

    word[0] = cmd;
    
    for (int i = 0; cmd[i] != '\0'; i++)
    {
        if (cond == SPACE)
        {
            if (isspace(cmd[i]))
            {
                cmd[i] = '\0';
                continue;
            }
            
            cond = NOT_SPACE;

            if (cmd[i] == CONVEYER)
            {
                word[++word_i] = nullptr;
                continue;
            }
            
            word[++word_i] = cmd + i;
        }
        else
        {
            if (isspace(cmd[i]))
            {
                cmd[i] = '\0';
                cond = SPACE;
            }
        }
    }
    if (cmd[0] == '\0')
        word_i -= 1;
    return word_i + 1;
}