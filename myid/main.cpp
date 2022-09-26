#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "ctype.h"
#include "errno.h"
#include "getopt.h"
#include "pwd.h"
#include "limits.h"
#include "grp.h"


#define PRINT(msg) printf( "%d \n", ( msg ) )

#define IF_ERRNO(expr, msg) if( errno )         \
                            {                   \
                                expr;           \
                                perror( msg );  \
                                return errno;   \
                            }


int idWithArgs( int argc, char** argv );

int idWithoutArgs();


int main( int argc, char** argv )
{
    errno = 0;

    if( argc == 1 )
        return idWithoutArgs();
    
    else
        return idWithArgs( argc, argv );
}


int idWithoutArgs()
{
    uid_t eff_user = geteuid();
    IF_ERRNO( , "in geteuid()" );

    gid_t eff_group = getegid();
    IF_ERRNO( , "in getegid()" );

    gid_t group_list[ NGROUPS_MAX ] = {};

    int ngroups = getgroups( NGROUPS_MAX, group_list );
    IF_ERRNO( , "in getgroups()" );

    struct passwd* user_pw = getpwuid( eff_user );
    IF_ERRNO( , "in getpwuid()" );

    char* username = user_pw->pw_name;

    struct group* group_gr = getgrgid( eff_group );
    IF_ERRNO( , "in getgpgid()" );

    char* groupname = group_gr->gr_name;

    if( !username && !groupname )
        return 1;

    printf( "uid=%d(%s) gid=%d(%s)"
            " groups=", eff_user, username, eff_group, groupname );

    for( int i = 0; i < ngroups; i++ )
    {
        group_gr = getgrgid( group_list[ i ] );
        IF_ERRNO( , "in loop in getgrgid()" );
        groupname = group_gr->gr_name;
        if( i == ngroups - 1 )
            printf( "%d(%s)", group_list[ i ], groupname );
        else
            printf( "%d(%s),", group_list[ i ], groupname );
    }

    return 0;
}


int IdWithArgs( int argc, char** argv )
{
    if( argc > 2 )
        return EINVAL;

    struct passwd* user = nullptr;
    
    if( !strIsDigit( argv[ 1 ] ) )
    {
        user = getpwnam( argv[ 1 ] );
        IF_ERRNO( , "in getpwnam()" );
    }
    else
    {
        int uid = atoi( argv[ 1 ] );
        user = getpwuid( uid );
        IF_ERRNO( , "in getpwuid()" );
    }

    int ngroups = -1;
    gid_t* groups = nullptr;
    gid_t group = 0;

    getgrouplist( argv[ 1 ], group, groups, ngroups );

    groups = ( gid_t* )calloc( ngroups, sizeof( gid_t ) );

    if( !groups )
        return -1;

    errno = 0;
    
    getgrouplist( argv[ 1 ], group, groups, ngroups );
    IF_ERRNO( , "in getgrouplist()" );

    struct group* gr = getgrgid( group );
    IF_ERRNO( , "in getgrgid()" );

    printf( "uid=%d(%s) gid=%d(%s) groups=", user->pw_uid, user->pw_name, gr->gr_gid, gr->gr_name );

    for( int i = 0; i < ngroups; i++ )
    {
        gr = getgrgid( groups[ i ] );
        IF_ERRNO( , "in loop in getgrgid()" );
        if( i + 1 = ngroups )
            printf( "%d(%s)", gr->gr_gid, gr->gr_name);
        else
            printf( "%d(%s),", gr->gr_gid, gr->gr_name);
    }

    return 0;
}


int strIsDigit( char* str )
{
    int i = 0;
    while( str[i] )
    {
        if( isdigit( str[ i ] ) )
            return 1;
        i++;
    }
    return 0;
}