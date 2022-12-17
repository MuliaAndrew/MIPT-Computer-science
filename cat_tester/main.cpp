#include <fcntl.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <limits.h>


const int BUFFER_SIZE = 4096;


struct Buf
{
    int fd_wr;
    int fd_rd;
    char * mem;
};

int main(int argc, char** argv)
{
    if (argc <= 2)
    {
        write(STDOUT_FILENO, "2 arguments required\n", 20);
        return 0;
    }
    
    char* cat_call = argv[1];
    int n_procs = atoi(argv[2]);

    if (n_procs >= FD_SETSIZE / 2)
    {
        printf("The amount of tests must be lower then FD_SETSIZE / 2 = %d", FD_SETSIZE / 2);
        return 0;
    }

    int * pipes_fd = (int *)calloc(4 * n_procs, sizeof(int));

    if (!pipes_fd)
    {
        fprintf(stderr, "calloc returned null");
        return 1;
    }

    for (int i = 0; i < n_procs * 2; i++)
    {
        if (pipe(pipes_fd + i * 2) == -1)
        {
            perror("in pipe()");
            return 1;
        }
    }

    pid_t pid;
    int index = 0;

    while (index < n_procs)
    {
        pid = fork();
        if (!pid)
            break;
        index++;
    }

    if (pid)
    {
        int flags = fcntl(0, F_GETFL);
        fcntl(0, F_SETFL, flags | O_NONBLOCK);
        
        flags = fcntl(1, F_GETFL);
        fcntl(1, F_SETFL, flags | O_NONBLOCK);

        for (int i = 0; i < n_procs * 4; i++)
        {
            flags = fcntl(pipes_fd[i], F_GETFL);
            fcntl(pipes_fd[i], F_SETFL, flags | O_NONBLOCK);

            if (errno)
            {
                perror("in fcntl()");
                kill(0, SIGKILL);
                return 1;
            }
        }

        for (int i = 0; i < n_procs * 4; i++)
        {
            close(pipes_fd[i * 4]);     // close pipe_to_cat[FD_READ]
            close(pipes_fd[i * 4 + 3]); // close pipe_from_cat]
        }

        char * memory = (char *)calloc((n_procs + 1) * BUFFER_SIZE, sizeof(char));
        
        Buf * buffers = (Buf *)calloc(n_procs + 1, sizeof(Buf));

        if (!memory || !buffers)
        {
            fprintf(stdout, "calloc() returned nullptr");
            kill(0, SIGKILL);
            return 1;
        }

        buffers[0].mem = memory;
        buffers[0].fd_rd = STDIN_FILENO;
        buffers[0].fd_wr = pipes_fd[1];

        buffers[n_procs].mem = memory + n_procs * BUFFER_SIZE;
        buffers[n_procs].fd_rd = pipes_fd[n_procs * 4 - 2];
        buffers[n_procs].fd_wr = STDOUT_FILENO;

        for (int i = 1; i < n_procs + 1; i++)
        {
            buffers[i].mem = memory + i * BUFFER_SIZE;
            buffers[i].fd_wr = pipes_fd[i * 4 + 1];
            buffers[i].fd_rd = pipes_fd[(i - 1) * 4 + 2];
        }

        int * sums = (int *)calloc(n_procs, sizeof(int));

        if (!sums)
        {
            fprintf(stdout, "calloc() returned nullptr");
            kill(0, SIGKILL);
            return 1;
        }

        read(buffers[0].fd_rd, buffers[0].mem, BUFFER_SIZE);

        fd_set read_fds;
        fd_set write_fds;
        timeval timeout = {0, 50};

        while (1)
        {
            FD_ZERO(&read_fds);
            FD_ZERO(&write_fds);

            for (int i = 0; i < n_procs + 1; i++) // boundary cases included
            {
                FD_SET(buffers[i].fd_rd, &read_fds);
                FD_SET(buffers[i].fd_wr, &write_fds);
            }
            // FD_SET(buffers[0].fd_wr, write_fds);
            // FD_SET(buffers[n_procs].fd_rd, read_fds);

            select(FD_SETSIZE, &read_fds, &write_fds, NULL, &timeout);

            int max_n_read = 0;
            int n_read;
            int max_n_writed = 0;
            int n_writed;

            for (int i = 1; i < n_procs; i++)
            {
                if (FD_ISSET(buffers[i].fd_rd, &read_fds))
                {
                    n_read = read(buffers[i].fd_rd, buffers[i].mem, BUFFER_SIZE);

                    if (n_read == -1)
                    {
                        perror("in read()");
                        kill(0, SIGKILL);
                        return 0;
                    }

                    if (n_read > max_n_read)
                        max_n_read = n_read;
                }

                if (FD_ISSET(buffers[i].fd_wr, &write_fds))
                {
                    n_writed = write(buffers[i].fd_wr, buffers[i].mem, BUFFER_SIZE);

                    if (n_writed == -1)
                    {
                        if (errno == EPIPE)
                        {
                            errno = 0;
                            n_writed = 0;
                        }

                        else
                        {
                            perror("in write()");
                            kill(0, SIGKILL);
                            return 1;
                        }
                    }
                }
            }

            // if (FD_ISSET(buffers[0].fd_wr, write_fds))
            // {

            // }

            // if (FD_ISSET(buffers[n_procs].fd_rd, read_fds))
            // {

            // }

            if (max_n_read & max_n_writed)
                break;
        }
    }

    else // child process
    {
        // fprintf(stderr, "[%d] index = %d\n", getpid(), index);

        for (int i = 0; i < n_procs; i++)
        {
            if (i == index)
                continue;
            
            close(pipes_fd[i * 4]);     // close pipe_to_cat[FD_READ] 
            close(pipes_fd[i * 4 + 1]); // close pipe_to_cat[FD_WRITE]
            close(pipes_fd[i * 4 + 2]); // close pipe_from_cat[FD_READ]
            close(pipes_fd[i * 4 + 3]); // close pipe_from_cat[FD_WRITE]
        }

        usleep(1);
        close(STDIN_FILENO);
        dup(pipes_fd[index * 4]);

        usleep(2);
        close(STDOUT_FILENO);
        dup(pipes_fd[index * 4 + 3]);

        close(pipes_fd[index * 4 + 1]); // close FD_WRITE in pipe_to_cat
        close(pipes_fd[index * 4 + 2]); // close FD_READ in pipe_from_cat

        execl(cat_call, "");
    }
}