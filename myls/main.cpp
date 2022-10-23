#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <string.h>


enum Flag
{
    all     = 0x01,
    dir     = 0x02,
    inode   = 0x04,
    lon     = 0x08,
    Recurs  = 0x10,
    numeric = 0x20,
};

void show(dirent* cur_e, int flags, char* name);

int getFlags(int argc, char** argv);

void ls(int flags, char* name);


int main(int argc, char** argv)
{
    int flags = getFlags(argc, argv);  

    if (optind == argc)
        ls(flags, ".");

    for (int i = optind; i < argc; i++)
        ls(flags, argv[i]);
    
    return 0;
}

int getFlags(int argc, char** argv)
{
    char short_flags[] = "ldaRin";

    const option long_flags[] = {
        {"all", no_argument, nullptr, 'a'},
        {"directory", no_argument, nullptr, 'd'},
        {"inode", no_argument, nullptr, 'i'},
        {"long", no_argument, nullptr, 'l'},
        {"recursive", no_argument, nullptr, 'R'},
        {"numeric-uid-gid", no_argument, nullptr, 'n'}
    };

    char cur_flag = 0x0;
    int flags = 0x0;

    while ((cur_flag = getopt_long(argc, argv, short_flags, long_flags, nullptr)) != -1)
    {
        switch (cur_flag)
        {
            case 'l':
                flags = flags | lon;
                break;
            case 'd':
                flags = flags | dir;
                break;
            case 'a':
                flags = flags | all;
                break;
            case 'R':
                flags = flags | Recurs;
                break;
            case 'i':
                flags = flags | inode;
                break;
            case 'n':
                flags = flags | numeric;
                break;
            case '?':
                perror("unknown flags");
                _exit(1);
            default:
                errno = EINVAL;
                perror("in getopt()");
                _exit(1);
        }
    }
    return flags;   
}

void ls(int flags, char* name)
{
    DIR* dir = opendir(name);

    dirent dirs[4096] = {}; // the queue of sirs to recursive mode
    int q_end = 0;

    if (errno)
    {
        perror("in opendir()");
        errno = 0;
        return;
    }

    if (flags & Recurs)
        printf("%s:\n", name);

    dirent* cur_e;

    while((cur_e = readdir(dir)))
    {
        if (!(flags & all) & cur_e->d_name[0] == '.')
            continue;

        char full_name[PATH_MAX] = {};
        strcpy(full_name, name);
        strcat(full_name, "/");
        strcat(full_name, cur_e->d_name);
        
        show(cur_e, flags, full_name);

        if ((flags & Recurs) && strcmp(cur_e->d_name, ".") && strcmp(cur_e->d_name, "..") && (cur_e->d_type & DT_DIR))
            dirs[q_end++] = *cur_e;

        errno = 0;
    }

    if (flags & Recurs)
        printf("\n\n");

    if (errno)
    {
        perror("in readdir()");
        _exit(-1);
    }

    if (flags & Recurs)
    {
        for (int i = 0; i < q_end; i++)
        {
            char next_dir[PATH_MAX] = {};
            strcpy(next_dir, name);
            strcat(next_dir, "/");
            strcat(next_dir, dirs[i].d_name);
            ls(flags, next_dir);
        }
    }

    closedir(dir);

    errno = 0;
}

void show(dirent* cur_e, int flags, char* name)
{   
    if (flags & lon)
    {   
        struct stat st_buf = {};

        if (stat(name, &st_buf))
        {
            perror("in stat()");
            _exit(errno);
        }

        if (flags & inode)
            printf("%ld ", st_buf.st_ino);
        
        char mode[10] = "rwxrwxrwx";
        char type;
        
        switch(st_buf.st_mode & S_IFMT)
        {
            case S_IFDIR:
                type = 'd';
                break;
            case S_IFREG:
                type = '-';
                break;
            case S_IFLNK:
                type = 'l';
                break;
            default:
                type = 'u'; // "udefined"
        }

        int mask = 0x01 << 8;

        for (int i = 0; i < 9; i++)
        {
            if (!(st_buf.st_mode & mask))
                mode[i] = '-';
            
            mask = mask >> 1;
        }

        uid_t own_uid = st_buf.st_uid;
        gid_t own_gid = st_buf.st_gid;

        struct passwd* u_pwd = getpwuid(own_gid);
        char* username = u_pwd->pw_name;

        struct group* gr = getgrgid(own_gid);
        char* grname = gr->gr_name;

        char* date = ctime(&st_buf.st_mtime);
        int date_len = strlen(date);
        date[date_len - 1] = '\0';

        printf("%c%s %ld %s %s %ld %s %s\n", type, mode, st_buf.st_nlink, username, grname, st_buf.st_size, date, cur_e->d_name);
    }
    else
        printf("%s\t", cur_e->d_name);
}