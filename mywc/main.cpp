#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


const int BUF_SIZE = 1024;

enum
{
    WORD,
    NOT_WORD
};

void count_words( char* buffer, int buf_size, int* nwords );

void count_lines( char* buffer, int buf_size, int* nlines );

int main( int agrc, char** argv )
{
    int pipefd[ 2 ] = {};

    if( pipe( pipefd ) )
        exit( -1 );

    pid_t pid = fork();

    if( !pid )
    {
        close( 1 );
        dup( pipefd[ 1 ] );
        close( pipefd[ 0 ] );
        execvp( argv[ 1 ], argv + 1 );
        perror( "exec() return -1" );
        exit( -1 );
    }
    else
    {
        close( pipefd[ 1 ] );

        char buffer[ BUF_SIZE ] = {};

        int n_bytes     = 0;
        int n_words     = 0;
        int n_lines     = 0;
        int buf_bytes   = 0;

        while( buf_bytes = read( pipefd[ 0 ], buffer, BUF_SIZE ) )
        {
            if( buf_bytes == -1 )
                perror( "in read()" );
                return -1;
                
            n_bytes += buf_bytes;
            count_words( buffer, BUF_SIZE, &n_words );
            count_lines( buffer, BUF_SIZE, &n_lines );
        }

        int status;
        wait( &status );

        printf( "Num of lines: %d; Num of words: %d; Num of bytes: %d\n", n_lines, n_words, n_bytes );
    }
}

void count_words( char* buffer, int buf_size, int* nwords )
{
    int mode = NOT_WORD;
    for( int i = 0; i < buf_size; i++ )
    {
        if( mode == NOT_WORD )
        {
            if( isspace( buffer[ i ] ) )
            {
                mode = WORD;
                *nwords += 1;
            }
            else
                continue;
        }
        else
        {
            if( isspace( buffer[ i ] ) )
                continue;
            else
                mode = NOT_WORD;
        }
    }
}

void count_lines( char* buffer, int buf_size, int* nlines )
{
    for( int i = 0; i < buf_size; i++ )
    {
        if( buffer[ i ] == '\n' )
            *nlines += 1;
    }
}