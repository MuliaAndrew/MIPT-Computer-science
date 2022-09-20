#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "getopt.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "fcntl.h"


#define IF_ERRNO(expr, msg) if( errno )         \
                            {                   \
                            expr;               \
                            perror( msg );      \
                            return errno;       \
                            }


const int BUFFER_SIZE = 1024;

const int FILE_ACCESS_MODES_MUSK = 0x0777;


enum flags 
{
    forse       = 0x01,
    verbose     = 0x02,
    interactive = 0x04,
};


int getFlags( int argc, char** argv );

int isDirectory( char* pathname );

int copyToFile( int argc, char** argv, int flags );

int copyToDirectory( int argc, char** argv, int flags );

char* getFileName( char* pathname );

char* createDestPathName( char* fname, char* dirpath );


int main( int argc, char** argv )
{   
    if( argc < 3 )
    {
        fprintf( stderr, "mycp requires at least two arguments" );
        return EINVAL;
    }

    int flags = getFlags( argc, argv );

    if( flags == -1 )
        return errno;

    if( !isDirectory( argv[ argc - 1 ] ) )
        return copyToDirectory( argc, argv, flags );
    else
        return copyToFile( argc, argv, flags );
}


int getFlags( int argc, char** argv )
{
    const char* short_flags = "fiv";

    const option long_flags[] = {
        { "forse", no_argument, nullptr, 'f' },
        { "interactive", no_argument, nullptr, 'i' },
        { "verbose", no_argument, nullptr, 'v' },
        { nullptr, 0, nullptr, 0 }
    };

    int flags = 0x00;
    char flag = 0;

    while( ( flag = getopt_long( argc, argv, short_flags, long_flags, nullptr ) ) != -1 )
    {
        switch ( flag )
        {
        case 'f':
            if( flags | forse )
            {
                fprintf( stderr, "mycp found two flags \'f\'." );
                return EINVAL;
            }
            flags = flags | forse;
            break;

        case 'v':
            if( flags | verbose )
            {
                fprintf( stderr, "mycp found two flags \'v\'." );
                return EINVAL;
            }
            flags = flags | verbose;
            break;

        case 'i':
            if( flags | interactive )
            {
                fprintf( stderr, "mycp found two flags \'i\'." );
                return EINVAL;
            }
            flags = flags | interactive;
            break;

        case '?':
            errno = EINVAL;
            perror( "found unknown flag" );
            return -1;

        default:
            perror( "mycp can not parse flags" );
            return -1;
        }
    }
    return flags;
}


int copyToFile( int argc, char** argv, int flags )
{
    const char buffer[ BUFFER_SIZE ] = { 0 };

    int n_files = 0;

    for( int i = 1; i < argc; i++ )
    {
        if( argv[ i ][ 0 ] != '-' )
            n_files++;
    }
    
    if( n_files != 2 )
    {
        errno = EINVAL;
        perror( "Bad arguments." );
        return errno;
    }
        
    int sourse_fd = open( argv[ argc - 2 ], O_RDONLY );

    IF_ERRNO( fprintf( stderr, "mycp can not read \'%s\'.", argv[ argc - 2 ] ), "" )

    struct stat sourse_stat = {};

    if( fstat( sourse_fd, &sourse_stat ) == -1 )
    {
        perror( "in fstat() in copyToFile()." );
        return errno;
    }

    mode_t dest_mode = sourse_stat.st_mode & FILE_ACCESS_MODES_MUSK | S_IWUSR;

    int dest_fd = -1;

    if ( !access( argv[ argc - 1 ], F_OK ) )
    {
        if( flags & interactive )
        {
            printf( "Do you want to truncate \'%s\'? ", argv[ argc - 1 ] );
            char response = 0;
            scanf( "%c", &response );

            switch( response )
            {
                case 'y': 
                    dest_fd = open( argv[ argc - 1 ], O_TRUNC | O_WRONLY );
                    break;

                case 'n':
                    return 0;

                default:
                    fprintf( stderr, "Undetermined response \'%c\'.", response );
                    return EINVAL;
            }
        }
        else
            dest_fd = open( argv[ argc - 1 ], O_TRUNC | O_WRONLY );
    }
    else
    {
        errno = 0;
        dest_fd = open( argv[ argc - 1 ], O_CREAT | O_EXCL | O_WRONLY, dest_mode );
    }

    if( flags & forse && dest_fd < 0)
    {
        if( remove( argv[ argc - 1 ] ) == -1 ) 
        {
            fprintf( stderr, " mycp can not remove \'%s\'.", argv[ argc - 1 ] );
            return errno;
        }

        dest_fd = open( argv[ argc - 1 ], O_CREAT | O_EXCL | O_WRONLY, dest_mode );
    }

    IF_ERRNO( fprintf( stderr, "mycp can not read \'%s\'.", argv[ argc - 1 ] ), "" )

    if( flags & verbose )
        printf( "\'%s\' -> \'%s\'\n", argv[ argc - 2 ], argv[ argc - 1 ] );

    while(1)
    {
        int n_read = 0;
        
        if( !( n_read = read( sourse_fd, ( char* )buffer, BUFFER_SIZE ) ) )
            break;
        
        IF_ERRNO( , "while reading sourse file." )

        write( dest_fd, ( char* )buffer, n_read );

        IF_ERRNO( , "while writing to dest file." )
    }

    close( sourse_fd );
    close( dest_fd );
    return 0;
}


int copyToDirectory( int argc, char** argv, int flags )
{
    char buffer[ BUFFER_SIZE ] = {};

    int ind = 1;

    while( argv[ ind ][ 0 ] == '-' )
        ind++;

    struct stat cur_stat = {};
    
    while( ind < argc - 1 )
    {
        int sourse_fd = open( argv[ ind ], O_RDONLY );

        IF_ERRNO( fprintf( stderr, "mycp can not open \'%s\'.", argv[ ind ] ), "" )

        if( fstat( sourse_fd, &cur_stat ) == -1 )
        {
            perror( "in fstat() in copyToDirectory()." );
            return errno;
        }

        mode_t dest_mode = cur_stat.st_mode & FILE_ACCESS_MODES_MUSK | S_IWUSR;

        int dest_fd = -1;

        char* fname = getFileName( argv[ ind ] );

        if( !fname )
            return -1;

        char* dest_pathname = createDestPathName( fname, argv[ argc - 1] );

        if( !dest_pathname )
            return -1;

        if ( !access( dest_pathname, F_OK ) )
        {
            if( flags & interactive )
            {
                printf( "Do you want to truncate \'%s\'? ", argv[ argc - 1 ] );
                char response = 0;
                scanf( "%c", &response );

                switch( response )
                {
                    case 'y': 
                        break;

                    case 'n':
                        return 0;

                    default:
                        fprintf( stderr, "Undetermined response \'%c\'.", response );
                        return EINVAL;
                }
            }
            dest_fd = open( dest_pathname, O_TRUNC | O_WRONLY );
        }
        else
        {
            errno = 0;
            dest_fd = open( dest_pathname, O_CREAT | O_EXCL | O_WRONLY, dest_mode );
        } 
        
        if( flags & forse && dest_fd < 0 )
        {
            if( remove( dest_pathname ) == -1 ) 
            {
                fprintf( stderr, " mycp can not remove \'%s\'.", dest_pathname );
                return errno;
            }
            errno = 0;

            dest_fd = open( dest_pathname, O_CREAT | O_EXCL | O_WRONLY, dest_mode );
        }

        IF_ERRNO( fprintf( stderr, "mycp can not open \'%s\'. fd = %d.", dest_pathname, dest_fd ) , "" );

        if( flags & verbose )
            printf( "\'%s\' -> \'%s\'\n", argv[ ind ], dest_pathname );

        while(1)
        {
            int n_read = 0;
            
            if( !( n_read = read( sourse_fd, buffer, BUFFER_SIZE ) ) )
                break;
            
            IF_ERRNO( fprintf( stderr, "While reading sourse file: \'%s\'.", argv[ ind ] ), "" )

            write( dest_fd, buffer, n_read );

            IF_ERRNO( fprintf( stderr, "While writing to dest file: \'%s\'.", dest_pathname ), "" ) 
        }

        free( fname );
        free( dest_pathname );

        close( sourse_fd );
        close( dest_fd );

        ind++;
    }

    return 0;
}


int isDirectory( char* pathname )
{
    if( !pathname )
    {
        fprintf( stderr, "zero pointer in isDirecory()." );
        return -1;
    }
    
    struct stat f_stat = {};
    
    if( stat( pathname, &f_stat ) == -1 && errno != ENOENT )
    {
            perror( "in isDirectory() in main()." );
            return errno;
    }

    if( f_stat.st_mode & S_IFDIR )
        return 0;
    else
        return -1;
}


char* getFileName( char* pathname )
{
    if( !pathname )
    {
        fprintf( stderr, "nullptr argument in getFileName()" );
        return nullptr;
    }
    
    int len = strlen( pathname );
    int i = len - 1;

    while( i > 0 && pathname[ i ] != '/')
        i--;
    
    char* name = ( char* )calloc( len - i, sizeof( char ) );

    if( !name )
    {
        fprintf( stderr, " allocator returned nullptr in getFileName()." );
        return nullptr;
    }
    
    name = strcpy( name, pathname + i );

    return name;
}


char* createDestPathName( char* fname, char* dirpath )
{
    if( !fname || !dirpath )
        return nullptr;
    
    int need_slash = 0;

    int dir_len = strlen( dirpath );
    int fname_len = strlen( fname );

    if( dirpath[ dir_len - 1 ] != '/' )
        need_slash = 1;
    
    char* res_name = ( char* )calloc( dir_len + need_slash + fname_len, sizeof( char ) );

    if( !res_name )
    {
        fprintf( stderr, "allocator returned nullptr in createDestPathName()" );
        return nullptr;
    }

    strcat( res_name, dirpath );

    if( need_slash )
        strcat( res_name, "/" );
    
    strcat( res_name, fname );

    return res_name;
}