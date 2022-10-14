#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "sys/time.h"

int main( int argc, char** argv )
{
    timeval start = {};
    timeval end = {};
    
    gettimeofday( &start, NULL );
    
    pid_t pid = fork();

    if( pid == 1 )
        return -1;
    
    if( !pid )
    {
        execvp( argv[ 1 ] , argv + 1 );
    }

    int status;
    wait( &status );
    gettimeofday( &end, NULL );

    timeval res = {};

    timersub( &end, &start, &res );

    printf( "%lds %ldus\n", res.tv_sec, res.tv_usec );

    return 0;
}