#pragma once

#include "view.h"

class TextView : public View 
{   
        static struct sigaction sig;

        enum LineDir { h, v };

        static void setWinSZSig();
        void clean();
        void putXY(unsigned int x, unsigned int y);
        void putSymb(char symb);
        void hline(unsigned int y, unsigned int length);
        void vline(unsigned int x, unsigned int length);
        void line(unsigned int x, unsigned int y, LineDir dir, unsigned int length, char symb);

    public:

        TextView();
        ~TextView();

        void draw();
        winsize getWinSize();
};

void winHndlr(int signum);

int getCtrlNum();