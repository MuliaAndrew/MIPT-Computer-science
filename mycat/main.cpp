#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const int BUFFER_SIZE = 1024;
const int ERROR_MSG_MAXLEN = 200;


int checkFiles( char** fnames );

int catWithArgs( int n_files, char** fnames );

int catWithoutArgs();


int main( int argc, char** argv )
{
    if( argc == 1 )
    {
        return catWithoutArgs();
    }
    
    int err = checkFiles( argv );
    
    if( err )
    {
        errno = ENOENT;

        char msg[ ERROR_MSG_MAXLEN ] = { 0 };
        strcat( msg, "File \'");
        strcat( msg, argv[ err ] );
        strcat( msg, "\' does not exist in this directory or unreadable.\0" );

        perror( msg );
    }

    catWithArgs( argc, argv );
}


int checkFiles( char** fnames )
{
    for( int i = 0; fnames[ i ] != NULL; i++ )
    {
        if( !access( fnames[ i ], R_OK ) )
            return i;
    }
    return 0;
}


int catWithoutArgs()
{
    char buffer[ BUFFER_SIZE ] = { 0 };

    while( 1 )
    {
        int readed = read( STDIN_FILENO, buffer, BUFFER_SIZE );
        
        if( !readed )
            return 0;
        
        if( errno  )
        {
            perror( "in fread() in catWithoutArgs()." );
            return 1;
        }

        if( write(STDOUT_FILENO, buffer, BUFFER_SIZE) == -1 )
        {
            perror( "in fwrite() in catWithoutArgs()" ); 
            return 1;
        }
    }
    return 0;
}


int catWithArgs( int n_files, char** fnames )
{
    char buffer[ BUFFER_SIZE ] = { 0 };

    int temp_errno = errno;

    for( int i = 1; i < n_files; i++ )
    {
        int cur_file = open( fnames[i], O_RDONLY );
        
        if ( !cur_file || errno != temp_errno )
        {
            perror( "in fopen() in catWithArgs()" );
            return 1;
        }

        while( 1 )
        {
            int readed = read( cur_file, buffer, BUFFER_SIZE );

            if( !readed )
                return 0;
            
            if( errno != temp_errno )
            {
                perror( "in read() in catWithArgs()" );
                return 1;
            }

            if( write(STDOUT_FILENO, buffer, BUFFER_SIZE) == -1 )
            {
                perror( "in fwrite() in catWithArgs()" );
                return 1;
            }
        }

        close( cur_file );
    }

    return 0;
}