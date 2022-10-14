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


        // printf("n_words: %d (%d bytes)\n", n_words, readBytes);

        if (!strcmp(word[0], EXIT_CMD))
            return 0;

        int source_offset = 0;
        int dest_offset = 0;
        int in_fd_prev = -1; // Input fd of pipe where previous command will write output
        int in_fd_next = -1; // Saved input fd of pipe for next command where it will write output
        int cur_pipe[2];
        // printf("source_offset = %d\n", source_offset);
        while(1)
        {   
            if (source_offset >= n_words || !n_words)
                break;
            
            for (int i = source_offset; word[i] != nullptr; i++)
                dest_offset++;
            
            dest_offset++;
            
            if (dest_offset < n_words)
            {
                if (pipe(cur_pipe) == -1)
                {
                    perror("in pipe()");
                    return errno;
                }
            }

            in_fd_next = cur_pipe[READ_PIPE];

            // printf("cur cmd: %s, args: %s, next cmd: %s\n", word[source_offset], word[source_offset + 1], word[dest_offset]);
            
            pid_t pid = fork();

            if (pid == -1)
            {
                perror("bad fork");
                _exit(1);
            }

            if (pid)
            {
                //printf("Parent: source_offset: %d, pid: %d, in_fd_prev: %d, in_fd_next: %d, pipe[READ_PIPE]: %d, pipe[WRITE_PIPE]: %d\n", 
                //        source_offset, pid, in_fd_prev, in_fd_next, cur_pipe[READ_PIPE], cur_pipe[WRITE_PIPE]);
                
                if (dest_offset < n_words)
                {
                    usleep(5);
                    close(cur_pipe[WRITE_PIPE]);
                }
                source_offset = dest_offset;
                // printf("dest_offset = %d\n", dest_offset);
                if (in_fd_prev != -1)
                {
                    usleep(6);
                    close(in_fd_prev);
                }
                in_fd_prev = in_fd_next;
                
            }
            else
            {
                //printf("Child: source_offset: %d, pid: %d, ppid: %d, in_fd_prev: %d, in_fd_next: %d, pipe[READ_PIPE]: %d, pipe[WRITE_PIPE]: %d\n", 
                //        source_offset, pid, getppid(), in_fd_prev, in_fd_next, cur_pipe[READ_PIPE], cur_pipe[WRITE_PIPE]);
                if (dest_offset < n_words)
                    usleep(1);
                    close(cur_pipe[READ_PIPE]);

                if (source_offset)
                {
                    usleep(2);
                    close(STDIN_FILENO);
                    dup(in_fd_prev);
                    //dup2(STDIN_FILENO, in_fd_prev)
                }
                if (dest_offset < n_words)
                {
                    usleep(3);
                    close(STDOUT_FILENO);
                    dup(cur_pipe[WRITE_PIPE]);
                }
                else
                {
                    if (cur_pipe[WRITE_PIPE])
                    {
                        usleep(4);
                        close(cur_pipe[WRITE_PIPE]);
                    }
                }
                //printf("Child : word[source_offset] = %s\n", word[source_offset]);
                if(execvp(word[source_offset], word + source_offset))
                {
                    fprintf(stderr, "Error occurs while executing %s command\n", word[source_offset]);
                    return errno;
                }
                else
                {
                    perror("exec");
                    _exit(-1);
                }
                printf("IM A CHILD");
            }
        }
        //close all FD
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