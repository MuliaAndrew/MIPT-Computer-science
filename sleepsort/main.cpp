#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"


int main( int argc, char** argv )
{
    for( int i = 1; i < argc; i++)
    {
        pid_t pid = fork();
        int num = atoi( argv[ i ] );

        switch( pid )
        {
            case -1:
                printf( "fork() = -1\n" );
                exit( 1 );
                
            case 0:
                usleep( 1000 * num );
                printf( "%d ", num);
                exit( 0 );

            default:
                break;
        }
    }

    for( int i = 1; i < argc; i++)
    {
        int status;
        wait( &status );
    }
    printf( "\n" );
    return 0;
}