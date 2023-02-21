#pragma once

#include <iostream>
#include <string>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>


class View 
{
        static View* obj;

    public:
        virtual ~View() {};

        static View* get(std::string &);
        virtual void draw() = 0;
        virtual winsize getWinSizw() = 0;
};