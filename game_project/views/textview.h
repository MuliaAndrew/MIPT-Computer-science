#pragma once

#include "view.h"

class TextView : public View 
{   
        static struct sigaction sig;

        enum LineDir { h, v };

        static void setWinSZSig();
        void clean();
        void putXY(unsigned int x, unsigned int y);
        void hline(unsigned int y, unsigned int length);
        void vline(unsigned int x, unsigned int length);
        void line(unsigned int x, unsigned int y, LineDir dir, unsigned int length);

    public:

        TextView();

        void draw();
        winsize getWinSizw();
};

void winHndlr(int signum);