#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>


#define READ_PIPE 0    // index of read fd in cur_pipe
#define WRITE_PIPE 1   // of write fd --||--


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
        
        if (errno && errno != ECHILD)
        {
            perror("in wait()");
            _exit(-1);
        }
        else
            errno = 0;
        
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
        int read_fd_prev = -1; // Input fd of pipe where previous command will write output
        int cur_pipe[2];

        while(dest_offset < n_words)
        {
            for(int i = source_offset; word[i] != nullptr; i++)
                dest_offset++;
            
            dest_offset++;

            if (dest_offset < n_words)
            {
                if (pipe(cur_pipe) == -1)
                {
                    perror("in pipe()");
                    _exit(-1);
                }
            }

            if (!source_offset && dest_offset < n_words)
                close(cur_pipe[READ_PIPE]);

            pid_t pid = fork();

            if (pid)
            {
                if (read_fd_prev != -1)
                    close(read_fd_prev);
                
                read_fd_prev = cur_pipe[READ_PIPE];

                if (cur_pipe[WRITE_PIPE] != 0)
                    close(cur_pipe[WRITE_PIPE]);
            }
            else
            {
                if (!source_offset && dest_offset > n_words)
                {
                    // write(STDOUT_FILENO, "1CASE\t", 7);

                    execvp(word[source_offset], word + source_offset);
                }
                else if (dest_offset > n_words && source_offset)
                {
                    // write(STDOUT_FILENO, "2CASE\t", 7);

                    close(STDIN_FILENO);
                    dup(cur_pipe[WRITE_PIPE]);
                    execvp(word[source_offset], word + source_offset);
                }
                else if (!source_offset && dest_offset < n_words)
                {
                    // write(STDOUT_FILENO, "3CASE\t", 7);

                    close(STDOUT_FILENO);
                    dup(cur_pipe[WRITE_PIPE]);
                    execvp(word[source_offset], word + source_offset);
                }
                else
                {
                    // write(STDOUT_FILENO, "4CASE\t", 7);

                    close(STDIN_FILENO);
                    dup(read_fd_prev);

                    close(STDOUT_FILENO);
                    dup(cur_pipe[WRITE_PIPE]);

                    close(cur_pipe[READ_PIPE]);

                    execvp(word[source_offset], word + source_offset);
                }
            }
        }
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