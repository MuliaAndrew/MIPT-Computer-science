#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "errno.h"

const int BUFFER_SIZE = 1024;
const int ERROR_MSG_MAXLEN = 200;


int checkFiles( char** fnames );

int catWithoutArgs();


int main( int argc, char** argv )
{
    // int errs = 0;

    if( argc == 1 )
    {
        return catWithoutArgs();
    }
    
    int err = checkFiles( argv );
    
    if( err )
    {
        int temp_errno = errno;
        errno = ENOENT;

        char msg[ ERROR_MSG_MAXLEN ] = { 0 };
        strcat( msg, "File \'");
        strcat( msg, argv[ err ] );
        strcat( msg, "\' does not exist in this directory or unreadable.\0" );

        perror( msg );

        errno = temp_errno;
    }

    // catWithArgs( argv );
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

    int temp_errno = errno;
    
    while( 1 )
    {
        int readed = fread( buffer, sizeof( char ), BUFFER_SIZE - 1, stdin );
        
        if( readed == 0 ) 
        {
            return 0;
        }
        
        if( errno != temp_errno )
        {
            perror( "in fread() in catWithoutArgs()." );
            return 1;
        }

        if( fwrite( buffer, sizeof( char ), readed, stdout ) <= 0 )
        {
            perror( "in fwrite() in catWithoutArgs()" ); 
            return 1;
        }
    }
    return 0;
}