#pragma once

#include "../models/model.h"

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
        static termios default_term_attrs;

    protected:
        static Models::Model* model;

    public:
        virtual ~View();

        void setonkey(int (*)(int));

        static View* get(std::string&);
        static void setModel(Models::Model&);
        virtual void draw() = 0;
        virtual winsize getWinSize() = 0;
};