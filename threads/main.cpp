#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


#define DEBUG_PRINT(msg) printf("%s\n", #msg)


#define ERROR_MSG_MAXLEN 1024
#define BUF_SIZE 1024
int poison = 15;


struct Monitor_t {
    char data[16 * BUF_SIZE];
    int  writer     = 0;
    int  reader     = 1;
    int  capacity   = 16;
    int  argc       = 0; 
    char** argv     = NULL;
    pthread_mutex_t * mutex;
    pthread_cond_t * wr, * rd;

    void init() {
        pthread_mutex_init(mutex, NULL);
        pthread_mutex_unlock(mutex);
        pthread_cond_init(wr, NULL);
        pthread_cond_init(rd, NULL);
    }

    char* buf_for_reader() {
        if (reader % capacity == writer % capacity)
            pthread_cond_wait(rd, mutex);
        int tmp = reader++;
        return data + (tmp % capacity) * BUF_SIZE;
    }

    char* buf_for_writer() {
        if ((writer + 1) % capacity == reader % capacity)
            pthread_cond_wait(wr, mutex);
        int tmp = writer++;
        return data + (tmp % capacity) * BUF_SIZE;
    }

    // Always release buffer that was returned by buf_for_writer()
    // Must be used only after buf_for_writer() after all operations with buffer
    void release_buf() {
        memset((char*)(data + ((writer - 1) % capacity) * BUF_SIZE), 0, BUF_SIZE);
        return;
    }
} FIFOm;


void * writer(void *);

void * reader(void *);

int checkFiles(char** fnames);


int main(int argc, char** argv) {
    DEBUG_PRINT(OK ACCESS);
    pthread_mutex_t _mutex;
    pthread_cond_t cond1, cond2;
    FIFOm.mutex = &_mutex;
    FIFOm.wr = &cond1;
    FIFOm.rd = &cond2;
    FIFOm.argv = argv;
    FIFOm.argc = argc;
    FIFOm.init();
    DEBUG_PRINT(OK INIT);

    int err;
    if (err = checkFiles(argv)) {
        errno = ENOENT;

        char msg[ERROR_MSG_MAXLEN] = {0};
        strcat(msg, "File \'");
        strcat(msg, argv[err]);
        strcat(msg, "\' does not exist in this directory or unreadable.\0");

        perror(msg);

        exit(EXIT_FAILURE);
    }

    pthread_t wr_thread;
    pthread_t rd_thread;

    if (pthread_create(&wr_thread, NULL, *writer, NULL)) {
        perror("in writer pthread");
        exit(EXIT_FAILURE);
    }
    DEBUG_PRINT(OK WR_THREAD CREATE);
    if (pthread_create(&rd_thread, NULL, *reader, NULL)) {
        perror("in reader pthread");
        exit(EXIT_FAILURE);
    }
    DEBUG_PRINT(OK RD_THREAD CREATE);
    pthread_join(wr_thread, NULL);
    pthread_join(rd_thread, NULL);

    exit(EXIT_SUCCESS);
}


void * writer(void * ptr) {
    DEBUG_PRINT(OK WRITER START);
    while (1) {
        char* cur_buf = FIFOm.buf_for_writer();
        DEBUG_PRINT(OK WRITER GET BUF);
        if (!write(STDOUT_FILENO, (char*)cur_buf, BUF_SIZE)) {
            FIFOm.release_buf();
            pthread_mutex_unlock(FIFOm.mutex);
            return NULL;
        }
        if (errno) {
            perror("in write()");
            return &poison;
        }
        FIFOm.release_buf();
        DEBUG_PRINT(OK WRITER REALESE BUF);
        pthread_cond_signal(FIFOm.rd);
    }
    return NULL;
}


void * reader(void * ptr) {
    DEBUG_PRINT(OK READER START);
    if (FIFOm.argc == 1) {
        while (1) {
            char* cur_buf = FIFOm.buf_for_reader();
            DEBUG_PRINT(OK READER GET BUFF);
            if (!read(STDIN_FILENO, (char*)cur_buf, BUF_SIZE)) {
                pthread_mutex_unlock(FIFOm.mutex);
                return NULL;
            }
            if (errno) {
                perror("in read() in reader");
                return &poison;
            }
            pthread_cond_signal(FIFOm.wr);
        }
    }

    for (int i = 1; i < FIFOm.argc; i++) {
        int cur_fd = open(FIFOm.argv[i], O_RDONLY);
        if (errno) {
            perror("in open()");
            return &poison;
        }
        while (1) {
            char* cur_buf = FIFOm.buf_for_reader();

            if (!read(cur_fd, (char*)cur_buf, BUF_SIZE)) {
                pthread_mutex_unlock(FIFOm.mutex);
                return NULL;
            }
            if (errno) {
                perror("in read()");
                return &poison;
            }
        }
        close(cur_fd);
    }
    return NULL;
}


int checkFiles(char** fnames) {
    for (int i = 1; fnames[i] != NULL; i++) {
        if (!access(fnames[i], R_OK))
            return i;
    }
    return 0;
}