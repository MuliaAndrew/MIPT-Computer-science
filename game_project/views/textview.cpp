#include "textview.h"

const char CSI[] = "\e[";

struct sigaction TextView::sig = {};

TextView::TextView()
{
    setWinSZSig();
}

void TextView::draw()
{   
    std::string buffer;
    int isOnPause = 0;

    for (int i = 0; i < 10; i++)
        model->createRabbitRandom();

    char ch = 0;
    while(true)
    {
        auto sz = getWinSize();
        model->setWinSZ(sz);

        auto readed = read(STDIN_FILENO, const_cast<char*>(buffer.c_str()), 4095);
        ch = buffer[0];

        for (auto f : HumanfonKeys)
            isOnPause = f(ch);

        if (!isOnPause)
        {
            clean();

            hline(0, sz.ws_col);
            hline(sz.ws_row, sz.ws_col);

            vline(0, sz.ws_row);
            vline(sz.ws_col, sz.ws_row);

            for (auto snake = model->snakes_begin(); snake != model->snakes_end(); snake++)
            {
                for (auto coord = snake->coords_begin(); coord != snake->coords_end(); coord++)
                {
                    putXY(coord->first, coord->second);
                    putSymb('@');
                }
            }

            for (auto rabbit = model->rabbits_begin(); rabbit != model->rabbits_end(); rabbit++)
            {
                auto coord = rabbit->coords();
                putXY(coord.first, coord.second);
                putSymb('$');
            }

            fflush(stdout);
        }
        else
        {
            if (model->isOnPause)
                return;
            else
                model->isOnPause = true;
        }
        
        usleep(model->getLoopPeriod());
        
        buffer.clear();
    }
}

winsize TextView::getWinSize()
{
    struct winsize sz;
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz))
    {
        perror("in TextView::getWinSizw()");
        return winsize{};
    }

    return sz;
}

void TextView::setWinSZSig()
{
    sigaction(SIGWINCH, nullptr, &sig);

    sig.sa_handler = &winHndlr;

    sigaction(SIGWINCH, &sig, nullptr);
}

void winHndlr(int signum)
{
    // std::string s = "";
    // auto view = View::get(s);

    // view->draw();
}

void TextView::clean()
{
    printf("%sH", CSI);
    printf("%sJ", CSI);
}

void TextView::putXY(unsigned int x, unsigned int y)
{
    printf("%s%u;%uH", CSI, x, y);
}

void TextView::putSymb(char symb)
{
    printf("%c", symb);
}

void TextView::line(unsigned int x, unsigned int y, LineDir dir, unsigned int length, char symb)
{
    if (dir >= 2)
        return;
    
    printf("%s%u;%uH", CSI, y, x);

    if (dir == h)
    {
        for (int i = 0; i < length; i++)
            putchar(symb);
    }
    else
    {
        for (int i = 0; i <= length; i++)
        {
            printf("%s%u;%uH", CSI, y + i, x);
            putchar(symb);
        }
    }
}

void TextView::vline(unsigned int x, unsigned int length)
{
    line(x, 0, LineDir::v, length, '#');
}

void TextView::hline(unsigned int y, unsigned int length)
{
    line(0, y, LineDir::h, length, '#');
}

